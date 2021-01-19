#include "MetaGenerator.h"

#include <_deps/antlr4/antlr4-runtime.h>
#include <UANTLR/ParserCpp14/CPP14Lexer.h>
#include <UANTLR/ParserCpp14/CPP14ParserBaseVisitor.h>

#include <sstream>

using namespace Ubpa::USRefl;
using namespace antlr4;

struct MetaGenerator::Impl : CPP14ParserBaseVisitor {
	std::string_view code;
	std::vector<AccessSpecifier> accessSpecifiers;
	std::vector<TypeMeta> typeMetas;
	std::vector<std::string> namespaces;
	std::vector<Parameter> templateParameters;
	bool inTemplateType{ false };

	struct Declarator {
		Field::Mode mode{ Field::Mode::Variable };
		std::vector<Attr> attrs;
		std::vector<std::string> pointerOperators; // *, &, &&
		bool isPacked{ false };
		std::string name;
		std::string initializer;
		std::vector<Parameter> parameters;
		std::vector<std::string> qualifiers; // const, volatile, &, &&, exception

		Field GenerateField() && {
			assert(!isPacked);
			
			Field field;
			
			field.mode = mode;
			field.name = std::move(name);
			field.attrs = std::move(attrs);
			field.parameters = std::move(parameters);
			field.qualifiers = std::move(qualifiers);
			field.initializer = std::move(initializer);
			field.pointerOperators = std::move(pointerOperators);
			
			return field;
		}
	};

	struct DeclSpecifierSeq {
		std::vector<DeclSpecifier> declSpecifiers;
		std::vector<Attr> attrs;
	};

	std::string GetTextPro(antlr4::ParserRuleContext* ctx) const {
		return {
			code.begin() + ctx->getStart()->getStartIndex(),
			code.begin() + ctx->getStop()->getStopIndex() + 1
		};
	}
	
	antlrcpp::Any visitChildren(antlr4::tree::ParseTree* node) override {
		antlrcpp::Any result = defaultResult();
		std::size_t n = node->children.size();
		for (std::size_t i = 0; i < n; i++) {
			if (!shouldVisitNextChild(node, result)) {
				break;
			}

			antlrcpp::Any childResult = node->children[i]->accept(this);
			result = std::move(childResult);// aggregateResult(result, childResult);
		}

		return result;
	}

	// attributeSpecifierSeq: attributeSpecifier+
	antlrcpp::Any visitAttributeSpecifierSeq(CPP14Parser::AttributeSpecifierSeqContext* ctx) override final {
		std::vector<Attr> attrs;
		for (auto* ctxAttributeSpecifier : ctx->attributeSpecifier()) {
			std::vector<Attr> partialAttrs = visit(ctxAttributeSpecifier);
			for (auto& attr : partialAttrs)
				attrs.push_back(std::move(attr));
		}
		return attrs;
	}

	// attributeSpecifier:
	//     LeftBracket LeftBracket(Using attributeNamespace Colon) ? attributeList ? RightBracket RightBracket
	//	   | alignmentspecifier
	antlrcpp::Any visitAttributeSpecifier(CPP14Parser::AttributeSpecifierContext* ctx) override final {
		std::vector<Attr> attrs;
		
		if (ctx->attributeList()) {
			for(auto* ctxAttr : ctx->attributeList()->attribute()) {
				Attr attr = visit(ctxAttr);
				attrs.push_back(std::move(attr));
			}
			if(ctx->attributeNamespace()) {
				const std::string ns = ctx->attributeNamespace()->getText();
				for(auto& attr : attrs)
					attr.ns = ns;
			}
		}
		
		return attrs;
	}

	antlrcpp::Any visitAttribute(CPP14Parser::AttributeContext* ctx) override final {
		Attr attr;
		if (ctx->attributeNamespace())
			attr.ns = ctx->attributeNamespace()->getText();
		attr.name = ctx->Identifier()->getText();
		if (ctx->attributeArgumentClause() && ctx->attributeArgumentClause()->balancedTokenSeq())
			attr.value = ctx->attributeArgumentClause()->balancedTokenSeq()->getText();
		return attr;
	}
	
	antlrcpp::Any visitTypeParameter(CPP14Parser::TypeParameterContext* ctx) override final {
		Parameter parameter;
		if (ctx->Typename_())
			parameter.type = "typename";
		else if (ctx->Class()) {
			if(ctx->templateparameterList()) {
				std::vector<Parameter> tparams = visit(ctx->templateparameterList());

				std::stringstream ss;
				ss << "template<";
				for (std::size_t i = 0; i < tparams.size(); i++) {
					ss << tparams[i].GenerateParameterName();
					if (i != tparams.size() - 1)
						ss << ", ";
				}
				ss << "> class";
				parameter.type = ss.str();
			}
			else
				parameter.type = "class";
		}
		else
			assert(false);
		
		if (ctx->Ellipsis())
			parameter.isPacked = true;
		if (ctx->Identifier())
			parameter.name = ctx->Identifier()->getText();
		return parameter;
	}

	antlrcpp::Any visitPointerAbstractDeclarator(CPP14Parser::PointerAbstractDeclaratorContext* ctx) override final {
		Declarator declarator;
		for (auto* ctxPointerOperator : ctx->pointerOperator())
			declarator.pointerOperators.push_back(ctxPointerOperator->getText());
		return declarator;
	}

	antlrcpp::Any visitAbstractPackDeclarator(CPP14Parser::AbstractPackDeclaratorContext* ctx) override final {
		Declarator declarator;
		declarator.isPacked = true;
		for (auto* ctxPointerOpeartor : ctx->pointerOperator())
			declarator.pointerOperators.push_back(ctxPointerOpeartor->getText());
		return declarator;
	}

	/*
	 * parameterDeclaration:
	 *     attributeSpecifierSeq? proDeclSpecifierSeq (
	 *         (declarator | abstractDeclarator)? ( Assign initializerClause )?
	 *     );
	 */
	antlrcpp::Any visitParameterDeclaration(CPP14Parser::ParameterDeclarationContext* ctx) override final {
		Parameter parameter;
		std::size_t start = ctx->proDeclSpecifierSeq()->preDeclSpecifierSeq() ?
			ctx->proDeclSpecifierSeq()->preDeclSpecifierSeq()->getStart()->getStartIndex()
			: ctx->proDeclSpecifierSeq()->proSimpleTypeSpecifier()->getStart()->getStartIndex();
		std::size_t stop = ctx->proDeclSpecifierSeq()->postDeclSpecifierSeq() ?
			ctx->proDeclSpecifierSeq()->postDeclSpecifierSeq()->getStop()->getStopIndex()
			: ctx->proDeclSpecifierSeq()->proSimpleTypeSpecifier()->getStop()->getStopIndex();
		parameter.type = { code.begin() + start, code.begin() + stop + 1 };
		
		if (ctx->declarator()) {
			Declarator declarator = visit(ctx->declarator());
			parameter.isPacked = declarator.isPacked;
			parameter.name = declarator.name;
			for (const auto& pointerOperator : declarator.pointerOperators)
				parameter.type += pointerOperator;
		}
		else if (ctx->abstractDeclarator()) {
			Declarator declarator = visit(ctx->abstractDeclarator());
			parameter.name = declarator.name;
			for (const auto& pointerOperator : declarator.pointerOperators)
				parameter.type += pointerOperator;
			parameter.isPacked = declarator.isPacked;
		}

		if(ctx->initializerClause())
			parameter.initializer = GetTextPro(ctx->initializerClause());
		
		return parameter;
	}
	
	/*
	 * enumSpecifier:
	 *     enumHead LeftBrace (enumeratorList Comma?)? RightBrace;
	 * enumHead:
	 *    enumkey attributeSpecifierSeq? (
	 *        nestedNameSpecifier? Identifier
	 *    )? enumbase?;
	 */
	antlrcpp::Any visitEnumSpecifier(CPP14Parser::EnumSpecifierContext* ctx) override final {
		if (inTemplateType)
			return {};

		if (!ctx->enumHead()->Identifier())
			return {};

		TypeMeta typeMeta;

		typeMeta.namespaces = namespaces;

		if (ctx->enumHead()->nestedNameSpecifier())
			typeMeta.name = GetTextPro(ctx->enumHead()->nestedNameSpecifier());

		typeMeta.name += ctx->enumHead()->Identifier()->getText();

		if(ctx->enumHead()->attributeSpecifierSeq())
			typeMeta.attrs = visit(ctx->enumHead()->attributeSpecifierSeq()).as<std::vector<Attr>>();

		if(ctx->enumeratorList())
			typeMeta.fields = visit(ctx->enumeratorList()).as<std::vector<Field>>();

		typeMetas.push_back(std::move(typeMeta));
		
		return {};
	}
	
	// enumeratorList: enumeratorDefinition(Comma enumeratorDefinition)*
	antlrcpp::Any visitEnumeratorList(CPP14Parser::EnumeratorListContext* ctx) override final {
		std::vector<Field> fields;
		for(auto* ctxEnumeratorDefinition : ctx->enumeratorDefinition()) {
			Field field = visit(ctxEnumeratorDefinition);
			fields.push_back(std::move(field));
		}
		return fields;
	}

	// enumeratorDefinition: enumerator attributeSpecifierSeq? (Assign constantExpression)?
	antlrcpp::Any visitEnumeratorDefinition(CPP14Parser::EnumeratorDefinitionContext* ctx) override final {
		Field field;
		field.mode = Field::Mode::Value;
		field.accessSpecifier = AccessSpecifier::PUBLIC;
		field.name = ctx->enumerator()->getText();
		if(ctx->attributeSpecifierSeq())
			field.attrs = visit(ctx->attributeSpecifierSeq()).as<std::vector<Attr>>();
		return field;
	}
	
	antlrcpp::Any visitTemplateDeclaration(CPP14Parser::TemplateDeclarationContext* ctx) override final {
		templateParameters = visit(ctx->templateparameterList()).as<std::vector<Parameter>>();

		auto rst = visitDeclaration(ctx->declaration());
		
		templateParameters.clear();

		if(rst.is<std::vector<Field>>()) {
			for (auto& field : rst.as<std::vector<Field>>())
				field.isTemplate = true;
		}
		else if (rst.is<Field>()) {
			Field field = rst;
			field.isTemplate = true;
			rst = std::vector<Field>{ field };
		}
		
		return rst;
	}

	antlrcpp::Any visitTemplateparameterList(CPP14Parser::TemplateparameterListContext* ctx) override final {
		std::vector<Parameter> rst;
		for (auto* ctxTemplateParameter : ctx->templateParameter()) {
			Parameter ele = visit(ctxTemplateParameter);
			rst.push_back(std::move(ele));
		}
		return rst;
	}

	antlrcpp::Any visitClassSpecifier(CPP14Parser::ClassSpecifierContext* ctx) override final {
		if (inTemplateType)
			return {};

		if (!accessSpecifiers.empty() && accessSpecifiers.back() != AccessSpecifier::PUBLIC)
			return {};
		
		TypeMeta typeMeta;
		
		typeMeta.namespaces = namespaces;
		typeMeta.name = ctx->classHead()->classHeadName()->getText();
		namespaces.push_back(typeMeta.name);

		if(ctx->classHead()->attributeSpecifierSeq())
			typeMeta.attrs = visit(ctx->classHead()->attributeSpecifierSeq()).as<std::vector<Attr>>();

		typeMeta.templateParameters = std::move(templateParameters);
		if (!typeMeta.templateParameters.empty())
			inTemplateType = true;

		if (ctx->classHead()->classKey()->getText() == "class") {
			accessSpecifiers.push_back(AccessSpecifier::PRIVATE);
			typeMeta.mode = TypeMeta::Mode::Class;
		}
		else {
			accessSpecifiers.push_back(AccessSpecifier::PUBLIC);
			typeMeta.mode = TypeMeta::Mode::Struct;
		}

		if(ctx->classHead()->baseClause())
			typeMeta.bases = visit(ctx->classHead()->baseClause()).as<std::vector<Base>>();

		if(ctx->memberSpecification()) {
			for (auto* child : ctx->memberSpecification()->children) {
				auto rst = visit(child);
				if (rst.is<std::vector<Field>>()) {
					for (auto& field : rst.as<std::vector<Field>>())
						typeMeta.fields.push_back(std::move(field));
				}
			}
		}

		typeMetas.push_back(std::move(typeMeta));

		accessSpecifiers.pop_back();
		namespaces.pop_back();
		inTemplateType = false;

		return {};
	}

	antlrcpp::Any visitBaseClause(CPP14Parser::BaseClauseContext* ctx) override final {
		std::vector<Base> bases;
		for(auto* ctxBaseSpecifierX : ctx->baseSpecifierList()->baseSpecifierX()) {
			Base base;
			if (ctxBaseSpecifierX->Ellipsis())
				base.isPacked = true;
			base.name = GetTextPro(ctxBaseSpecifierX->baseSpecifier()->baseTypeSpecifier());
			if (ctxBaseSpecifierX->baseSpecifier()->Virtual())
				base.isVirtual = true;
			if (auto ctxAccessSpecifier = ctxBaseSpecifierX->baseSpecifier()->accessSpecifier()) {
				const auto accessSpecifier = ctxAccessSpecifier->getText();
				if (accessSpecifier == "public")
					base.accessSpecifier = AccessSpecifier::PUBLIC;
				else if (accessSpecifier == "protected")
					base.accessSpecifier = AccessSpecifier::PROTECTED;
				else
					base.accessSpecifier = AccessSpecifier::PRIVATE;
			}
			bases.push_back(std::move(base));
		}
		return bases;
	}
	
	antlrcpp::Any visitAccessSpecifier(CPP14Parser::AccessSpecifierContext* ctx) override final {
		if (!accessSpecifiers.empty()) {
			if (ctx->Public())
				accessSpecifiers.back() = AccessSpecifier::PUBLIC;
			else if (ctx->Protected())
				accessSpecifiers.back() = AccessSpecifier::PROTECTED;
			else
				accessSpecifiers.back() = AccessSpecifier::PRIVATE;
		}

		return {};
	}

	antlrcpp::Any visitNamespaceDefinition(CPP14Parser::NamespaceDefinitionContext* ctx) override final {
		const auto origNamespacesSize = namespaces.size();

		if(ctx->Identifier())
			namespaces.push_back(ctx->Identifier()->getText());
		else if (ctx->originalNamespaceName()) {
			std::vector<std::string> curNamespaces = visitOriginalNamespaceName(ctx->originalNamespaceName());
			for (auto& ns : curNamespaces)
				namespaces.push_back(std::move(ns));
		}

		visit(ctx->declarationseq());

		while (namespaces.size() > origNamespacesSize)
			namespaces.pop_back();

		return {};
	}

	antlrcpp::Any visitNestedNameSpecifier(CPP14Parser::NestedNameSpecifierContext* ctx) override final {
		if (ctx->nestedNameSpecifier()) {
			std::vector<std::string> curNamespaces = visitNestedNameSpecifier(ctx->nestedNameSpecifier());
			curNamespaces.push_back(ctx->Identifier()->getText());
			return curNamespaces;
		}
		else
			return std::vector<std::string>{ctx->theTypeName()->getText()};
	}

	antlrcpp::Any visitQualifiedNamespaceSpecifier(CPP14Parser::QualifiedNamespaceSpecifierContext* ctx) override final {
		if (ctx->nestedNameSpecifier()) {
			std::vector<std::string> curNamespaces = visitNestedNameSpecifier(ctx->nestedNameSpecifier());
			curNamespaces.push_back(ctx->namespaceName()->getText());
			return curNamespaces;
		}
		else
			return std::vector<std::string>{ctx->namespaceName()->getText()};
	}

	antlrcpp::Any visitProDeclSpecifierSeq(CPP14Parser::ProDeclSpecifierSeqContext* ctx) override final {
		DeclSpecifierSeq seq;
		if (auto* pre = ctx->preDeclSpecifierSeq()) {
			for (auto* ctxDeclSpecifier : pre->nonSimpleTypeDeclSpecifier()) {
				DeclSpecifier declSpecifier = GetTextPro(ctxDeclSpecifier);
				seq.declSpecifiers.push_back(declSpecifier);
			}
		}
		{
			DeclSpecifier declSpecifier = GetTextPro(ctx->proSimpleTypeSpecifier());
			seq.declSpecifiers.push_back(declSpecifier);
		}
		if (auto* post = ctx->postDeclSpecifierSeq()) {
			for (auto* ctxDeclSpecifier : post->nonSimpleTypeDeclSpecifier()) {
				DeclSpecifier declSpecifier = GetTextPro(ctxDeclSpecifier);
				seq.declSpecifiers.push_back(declSpecifier);
			}
		}
		if (ctx->attributeSpecifierSeq()) {
			std::vector<Attr> attrs = visit(ctx->attributeSpecifierSeq());
			for (auto& attr : attrs)
				seq.attrs.push_back(std::move(attr));
		}
		return seq;
	}

	antlrcpp::Any visitMemberDeclaration(CPP14Parser::MemberDeclarationContext* ctx) override final {
		if (ctx->emptyDeclaration() || ctx->aliasDeclaration() || ctx->staticAssertDeclaration() || ctx->usingDeclaration())
			return {};
		
		if (ctx->declSpecifierSeq())
			return visit(ctx->declSpecifierSeq());
		if (ctx->templateDeclaration())
			return visit(ctx->templateDeclaration());
		if (ctx->functionDefinition()) {
			Field field = visit(ctx->functionDefinition());
			return std::vector<Field>{field};
		}

		std::vector<Attr> commonAttrs;
		std::vector<DeclSpecifier> declSpecifiers;
		if(auto* pro = ctx->proDeclSpecifierSeq()) {
			DeclSpecifierSeq seq = visit(pro);
			declSpecifiers = std::move(seq.declSpecifiers);
			commonAttrs = std::move(seq.attrs);
		}
		if(ctx->attributeSpecifierSeq()) {
			std::vector<Attr> attrs = visit(ctx->attributeSpecifierSeq());
			for (auto& attr : attrs)
				commonAttrs.push_back(std::move(attr));
		}
		
		std::vector<Field> fields;

		if (ctx->memberDeclaratorList()) {
			std::vector<Declarator> declarators = visit(ctx->memberDeclaratorList());
			for (auto& declarator : declarators) {
				Field field = std::move(declarator).GenerateField();
				if (!accessSpecifiers.empty())
					field.accessSpecifier = accessSpecifiers.back();
				field.declSpecifiers = declSpecifiers;
				if (field.IsStaticConstexprVariable())
					field.mode = Field::Mode::Value;
				for (const auto& attr : commonAttrs)
					field.attrs.push_back(attr);
				fields.push_back(std::move(field));
			}
		}
		
		return fields;
	}

	antlrcpp::Any visitMemberDeclaratorList(CPP14Parser::MemberDeclaratorListContext* ctx) override final {
		std::vector<Declarator> declarators;
		for(auto* ctxMemberDeclarator : ctx->memberDeclarator()) {
			Declarator declarator = visitMemberDeclarator(ctxMemberDeclarator);
			declarators.push_back(std::move(declarator));
		}
		return declarators;
	}

	/*
	 * memberDeclarator:
	 * declarator (
	 *     virtualSpecifierSeq? pureSpecifier?
	 *     | braceOrEqualInitializer?
	 * )
	 * | Identifier? attributeSpecifierSeq? Colon constantExpression;
	 */
	antlrcpp::Any visitMemberDeclarator(CPP14Parser::MemberDeclaratorContext* ctx) override final {
		Declarator declarator;
		if (ctx->declarator())
			declarator = visit(ctx->declarator());
		else if (ctx->Identifier()) {
			declarator.name = ctx->Identifier()->getText();
			if (ctx->attributeSpecifierSeq())
				declarator.attrs = visit(ctx->attributeSpecifierSeq()).as<std::vector<Attr>>();
		}
		else
			assert(false);

		if (ctx->braceOrEqualInitializer())
			declarator.initializer = visit(ctx->braceOrEqualInitializer()).as<std::string>();
		
		return declarator;
	}

	antlrcpp::Any visitPointerDeclarator(CPP14Parser::PointerDeclaratorContext* ctx) override final {
		Declarator declarator = visit(ctx->noPointerDeclarator());
		for(auto* ctxPointerOpeartor:ctx->pointerOperator())
			declarator.pointerOperators.push_back(ctxPointerOpeartor->getText());
		return declarator;
	}

	antlrcpp::Any visitNoPointerDeclarator(CPP14Parser::NoPointerDeclaratorContext* ctx) override final {
		Declarator declarator = ctx->noPointerDeclarator()?
			visit(ctx->noPointerDeclarator()).as<Declarator>()
			: Declarator{};

		if (ctx->declaratorid()) {
			declarator.name = ctx->declaratorid()->idExpression()->getText();
			declarator.isPacked = ctx->declaratorid()->Ellipsis() != nullptr;
		}

		if (ctx->attributeSpecifierSeq()) {
			std::vector<Attr> attrs = visit(ctx->attributeSpecifierSeq());
			for (auto& attr : attrs)
				declarator.attrs.push_back(std::move(attr));
		}
		
		if(ctx->parametersAndQualifiers()) {
			declarator.mode = Field::Mode::Function;
			if(ctx->parametersAndQualifiers()->attributeSpecifierSeq()) {
				std::vector<Attr> attrs = visit(ctx->parametersAndQualifiers()->attributeSpecifierSeq());
				for (auto& attr : attrs)
					declarator.attrs.push_back(std::move(attr));
			}
			if(auto* ctxClause = ctx->parametersAndQualifiers()->parameterDeclarationClause()) {
				for(auto* ctxParameterDeclaration : ctxClause->parameterDeclarationList()->parameterDeclaration()) {
					Parameter parameter = visit(ctxParameterDeclaration);
					declarator.parameters.push_back(std::move(parameter));
				}
			}
			if(auto* ctxCVSeq = ctx->parametersAndQualifiers()->cvqualifierseq()) {
				for (auto* ctxCV : ctxCVSeq->cvQualifier())
					declarator.qualifiers.push_back(ctxCV->getText());
			}
			if (auto* ctxRef = ctx->parametersAndQualifiers()->refqualifier())
				declarator.qualifiers.push_back(ctxRef->getText());

			if (ctx->parametersAndQualifiers()->exceptionSpecification())
				declarator.qualifiers.push_back(ctx->parametersAndQualifiers()->exceptionSpecification()->getText());
		}

		return declarator;
	}

	antlrcpp::Any visitBraceOrEqualInitializer(CPP14Parser::BraceOrEqualInitializerContext* ctx) override final {
		std::string rst;
		if(ctx->initializerClause())
			rst = GetTextPro(ctx->initializerClause());
		else
			rst = GetTextPro(ctx->bracedInitList());
		return rst;
	}

	antlrcpp::Any visitSimpleDeclaration(CPP14Parser::SimpleDeclarationContext* ctx) override final {
		if (!ctx->initDeclaratorList())
			return visitChildren(ctx);
		
		std::vector<Attr> commonAttrs;
		std::vector<DeclSpecifier> declSpecifiers;
		if (ctx->proDeclSpecifierSeq()) {
			DeclSpecifierSeq seq = visit(ctx->proDeclSpecifierSeq());
			declSpecifiers = std::move(seq.declSpecifiers);
			commonAttrs = std::move(seq.attrs);
		}
		if (ctx->attributeSpecifierSeq()) {
			std::vector<Attr> attrs = visit(ctx->attributeSpecifierSeq());
			for (auto& attr : attrs)
				commonAttrs.push_back(std::move(attr));
		}

		std::vector<Field> fields;

		std::vector<Declarator> declarators = visit(ctx->initDeclaratorList());
		for (auto& declarator : declarators) {
			Field field = std::move(declarator).GenerateField();
			field.declSpecifiers = declSpecifiers;
			if (!accessSpecifiers.empty())
				field.accessSpecifier = accessSpecifiers.back();
			if (field.IsStaticConstexprVariable())
				field.mode = Field::Mode::Value;
			for (const auto& attr : commonAttrs)
				field.attrs.push_back(attr);
			fields.push_back(std::move(field));
		}

		return fields;
	}

	// initDeclaratorList: initDeclarator (Comma initDeclarator)*;
	antlrcpp::Any visitInitDeclaratorList(CPP14Parser::InitDeclaratorListContext* ctx) override final {
		std::vector<Declarator> declarators;
		for(auto* ctxInitDeclarator : ctx->initDeclarator()) {
			Declarator declarator = visit(ctxInitDeclarator);
			declarators.push_back(std::move(declarator));
		}
		return declarators;
	}

	// initDeclarator: declarator initializer?;
	antlrcpp::Any visitInitDeclarator(CPP14Parser::InitDeclaratorContext* ctx) override final {
		Declarator declarator = visit(ctx->declarator());
		return declarator;
	}

	// functionDefinition: attributeSpecifierSeq ? proDeclSpecifierSeq ? declarator virtualSpecifierSeq ? functionBody;
	antlrcpp::Any visitFunctionDefinition(CPP14Parser::FunctionDefinitionContext* ctx) override final {
		Declarator declarator = visit(ctx->declarator());
		Field field = std::move(declarator).GenerateField();
		if (!accessSpecifiers.empty())
			field.accessSpecifier = accessSpecifiers.back();
		field.mode = Field::Mode::Function;
		if (ctx->attributeSpecifierSeq())
			field.attrs = visit(ctx->attributeSpecifierSeq()).as<std::vector<Attr>>();
		if (ctx->proDeclSpecifierSeq()) {
			DeclSpecifierSeq seq = visit(ctx->proDeclSpecifierSeq());
			field.declSpecifiers = std::move(seq.declSpecifiers);
			for (auto& attr : seq.attrs)
				field.attrs.push_back(std::move(attr));
		}
		if (ctx->functionBody()->Delete())
			field.initializer = "delete";
		return field;
	}
};

MetaGenerator::MetaGenerator()
	: pImpl{ std::make_unique<MetaGenerator::Impl>() } {}

MetaGenerator::~MetaGenerator() = default;
MetaGenerator::MetaGenerator(MetaGenerator&&) noexcept = default;
MetaGenerator& MetaGenerator::operator=(MetaGenerator&&) noexcept = default;

std::vector<TypeMeta> MetaGenerator::Parse(std::string_view cppCode) {
	std::vector<TypeMeta> rst;

	// [0. init]
	pImpl->code = cppCode;
	
	// [1. parse]
	ANTLRInputStream input(cppCode.data());
	CPP14Lexer lexer(&input);
	CommonTokenStream tokens(&lexer);

	CPP14Parser parser(&tokens);
	tree::ParseTree* tree = parser.translationUnit();

	tree->accept(pImpl.get());

	// [2. clear]
	rst = std::move(pImpl->typeMetas);
	pImpl->code = {};
	assert(pImpl->accessSpecifiers.empty());
	assert(pImpl->namespaces.empty());
	assert(!pImpl->inTemplateType);
	assert(pImpl->templateParameters.empty());
	assert(pImpl->typeMetas.empty());

	// [3. return]
	return rst;
}
