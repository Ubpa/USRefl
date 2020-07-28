#pragma once

#include <antlr4-runtime.h>
#include <UANTLR/ParserCpp14/CPP14Visitor.h>
#include <map>
#include <string>
#include <vector>

namespace Ubpa::USRefl {
	class AutoRefl : public CPP14Visitor {
	public:
		// class code -> reflection code
		std::string Parse(std::string_view code);

	private:
		enum class AccessSpecifier {
			PUBLIC,
			PROTECTED,
			PRIVATE
		};
		struct FieldInfo {
			std::string name;
			std::map<std::string, std::string> metas;
			AccessSpecifier access;
			bool isFunc{ false };
		};
		struct VarInfo {
			std::string name;
			std::map<std::string, std::string> metas;
			AccessSpecifier access;

			// - storage class specifier: register, static, thread_local, extern, mutable
			// - function specifier: inline, virtual, explicit
			// - friend
			// - typedef
			// - constexpr
			std::vector<std::string> specifiers;
		};
		struct FuncInfo {
			std::string name;
			std::map<std::string, std::string> metas;
			AccessSpecifier access;
		};
		struct TypeInfo {
			std::vector<std::string> ns; // namespace
			std::string classkey; // class / struct
			std::string name; // name
			std::map<std::string, std::string> metas;
			std::vector<VarInfo> varInfos;
			std::vector<FuncInfo> funcInfos;
		};

		std::vector<TypeInfo> typeInfos;
		std::vector<std::string> curNamespace;
		std::map<std::string, std::string>* curMetas{ nullptr };
		TypeInfo* curTypeInfo{ nullptr };
		VarInfo* curVarInfo{ nullptr };
		FuncInfo* curFuncInfo{ nullptr };
		AccessSpecifier curAccessSpecifier{ AccessSpecifier::PRIVATE };
		FieldInfo curFieldInfo;
		bool inMember{ false };

		virtual antlrcpp::Any visitTranslationunit(CPP14Parser::TranslationunitContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitPrimaryexpression(CPP14Parser::PrimaryexpressionContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitIdexpression(CPP14Parser::IdexpressionContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitUnqualifiedid(CPP14Parser::UnqualifiedidContext* ctx) override;

		virtual antlrcpp::Any visitQualifiedid(CPP14Parser::QualifiedidContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitNestednamespecifier(CPP14Parser::NestednamespecifierContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitLambdaexpression(CPP14Parser::LambdaexpressionContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitLambdaintroducer(CPP14Parser::LambdaintroducerContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitLambdacapture(CPP14Parser::LambdacaptureContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitCapturedefault(CPP14Parser::CapturedefaultContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitCapturelist(CPP14Parser::CapturelistContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitCapture(CPP14Parser::CaptureContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitSimplecapture(CPP14Parser::SimplecaptureContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitInitcapture(CPP14Parser::InitcaptureContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitLambdadeclarator(CPP14Parser::LambdadeclaratorContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitPostfixexpression(CPP14Parser::PostfixexpressionContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitTypeidofexpr(CPP14Parser::TypeidofexprContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitTypeidofthetypeid(CPP14Parser::TypeidofthetypeidContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitExpressionlist(CPP14Parser::ExpressionlistContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitPseudodestructorname(CPP14Parser::PseudodestructornameContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitUnaryexpression(CPP14Parser::UnaryexpressionContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitUnaryoperator(CPP14Parser::UnaryoperatorContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitNewexpression(CPP14Parser::NewexpressionContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitNewplacement(CPP14Parser::NewplacementContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitNewtypeid(CPP14Parser::NewtypeidContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitNewdeclarator(CPP14Parser::NewdeclaratorContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitNoptrnewdeclarator(CPP14Parser::NoptrnewdeclaratorContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitNewinitializer(CPP14Parser::NewinitializerContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitDeleteexpression(CPP14Parser::DeleteexpressionContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitNoexceptexpression(CPP14Parser::NoexceptexpressionContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitCastexpression(CPP14Parser::CastexpressionContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitPmexpression(CPP14Parser::PmexpressionContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitMultiplicativeexpression(CPP14Parser::MultiplicativeexpressionContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitAdditiveexpression(CPP14Parser::AdditiveexpressionContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitShiftexpression(CPP14Parser::ShiftexpressionContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitShiftoperator(CPP14Parser::ShiftoperatorContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitRelationalexpression(CPP14Parser::RelationalexpressionContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitEqualityexpression(CPP14Parser::EqualityexpressionContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitAndexpression(CPP14Parser::AndexpressionContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitExclusiveorexpression(CPP14Parser::ExclusiveorexpressionContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitInclusiveorexpression(CPP14Parser::InclusiveorexpressionContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitLogicalandexpression(CPP14Parser::LogicalandexpressionContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitLogicalorexpression(CPP14Parser::LogicalorexpressionContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitConditionalexpression(CPP14Parser::ConditionalexpressionContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitAssignmentexpression(CPP14Parser::AssignmentexpressionContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitAssignmentoperator(CPP14Parser::AssignmentoperatorContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitExpression(CPP14Parser::ExpressionContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitConstantexpression(CPP14Parser::ConstantexpressionContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitStatement(CPP14Parser::StatementContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitLabeledstatement(CPP14Parser::LabeledstatementContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitExpressionstatement(CPP14Parser::ExpressionstatementContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitCompoundstatement(CPP14Parser::CompoundstatementContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitStatementseq(CPP14Parser::StatementseqContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitSelectionstatement(CPP14Parser::SelectionstatementContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitCondition(CPP14Parser::ConditionContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitIterationstatement(CPP14Parser::IterationstatementContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitForinitstatement(CPP14Parser::ForinitstatementContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitForrangedeclaration(CPP14Parser::ForrangedeclarationContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitForrangeinitializer(CPP14Parser::ForrangeinitializerContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitJumpstatement(CPP14Parser::JumpstatementContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitDeclarationstatement(CPP14Parser::DeclarationstatementContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitDeclarationseq(CPP14Parser::DeclarationseqContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitDeclaration(CPP14Parser::DeclarationContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitBlockdeclaration(CPP14Parser::BlockdeclarationContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitAliasdeclaration(CPP14Parser::AliasdeclarationContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitSimpledeclaration(CPP14Parser::SimpledeclarationContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitStatic_assertdeclaration(CPP14Parser::Static_assertdeclarationContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitEmptydeclaration(CPP14Parser::EmptydeclarationContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitAttributedeclaration(CPP14Parser::AttributedeclarationContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitDeclspecifier(CPP14Parser::DeclspecifierContext* ctx) override;

		virtual antlrcpp::Any visitDeclspecifierseq(CPP14Parser::DeclspecifierseqContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitStorageclassspecifier(CPP14Parser::StorageclassspecifierContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitFunctionspecifier(CPP14Parser::FunctionspecifierContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitTypedefname(CPP14Parser::TypedefnameContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitTypespecifier(CPP14Parser::TypespecifierContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitTrailingtypespecifier(CPP14Parser::TrailingtypespecifierContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitTypespecifierseq(CPP14Parser::TypespecifierseqContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitTrailingtypespecifierseq(CPP14Parser::TrailingtypespecifierseqContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitSimpletypespecifier(CPP14Parser::SimpletypespecifierContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitThetypename(CPP14Parser::ThetypenameContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitDecltypespecifier(CPP14Parser::DecltypespecifierContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitElaboratedtypespecifier(CPP14Parser::ElaboratedtypespecifierContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitEnumname(CPP14Parser::EnumnameContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitEnumspecifier(CPP14Parser::EnumspecifierContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitEnumhead(CPP14Parser::EnumheadContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitOpaqueenumdeclaration(CPP14Parser::OpaqueenumdeclarationContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitEnumkey(CPP14Parser::EnumkeyContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitEnumbase(CPP14Parser::EnumbaseContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitEnumeratorlist(CPP14Parser::EnumeratorlistContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitEnumeratordefinition(CPP14Parser::EnumeratordefinitionContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitEnumerator(CPP14Parser::EnumeratorContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitNamespacename(CPP14Parser::NamespacenameContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitOriginalnamespacename(CPP14Parser::OriginalnamespacenameContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitNamespacedefinition(CPP14Parser::NamespacedefinitionContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitNamednamespacedefinition(CPP14Parser::NamednamespacedefinitionContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitOriginalnamespacedefinition(CPP14Parser::OriginalnamespacedefinitionContext* ctx) override;

		virtual antlrcpp::Any visitExtensionnamespacedefinition(CPP14Parser::ExtensionnamespacedefinitionContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitUnnamednamespacedefinition(CPP14Parser::UnnamednamespacedefinitionContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitNamespacebody(CPP14Parser::NamespacebodyContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitNamespacealias(CPP14Parser::NamespacealiasContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitNamespacealiasdefinition(CPP14Parser::NamespacealiasdefinitionContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitQualifiednamespacespecifier(CPP14Parser::QualifiednamespacespecifierContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitUsingdeclaration(CPP14Parser::UsingdeclarationContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitUsingdirective(CPP14Parser::UsingdirectiveContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitAsmdefinition(CPP14Parser::AsmdefinitionContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitLinkagespecification(CPP14Parser::LinkagespecificationContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitAttributespecifierseq(CPP14Parser::AttributespecifierseqContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitAttributespecifier(CPP14Parser::AttributespecifierContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitAlignmentspecifier(CPP14Parser::AlignmentspecifierContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitAttributelist(CPP14Parser::AttributelistContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitAttribute(CPP14Parser::AttributeContext* ctx) override;

		virtual antlrcpp::Any visitAttributetoken(CPP14Parser::AttributetokenContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitAttributescopedtoken(CPP14Parser::AttributescopedtokenContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitAttributenamespace(CPP14Parser::AttributenamespaceContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitAttributeargumentclause(CPP14Parser::AttributeargumentclauseContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitBalancedtokenseq(CPP14Parser::BalancedtokenseqContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitBalancedtoken(CPP14Parser::BalancedtokenContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitInitdeclaratorlist(CPP14Parser::InitdeclaratorlistContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitInitdeclarator(CPP14Parser::InitdeclaratorContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitDeclarator(CPP14Parser::DeclaratorContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitPtrdeclarator(CPP14Parser::PtrdeclaratorContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitNoptrdeclarator(CPP14Parser::NoptrdeclaratorContext* ctx) override;

		virtual antlrcpp::Any visitParametersandqualifiers(CPP14Parser::ParametersandqualifiersContext* ctx) override;

		virtual antlrcpp::Any visitTrailingreturntype(CPP14Parser::TrailingreturntypeContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitPtroperator(CPP14Parser::PtroperatorContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitCvqualifierseq(CPP14Parser::CvqualifierseqContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitCvqualifier(CPP14Parser::CvqualifierContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitRefqualifier(CPP14Parser::RefqualifierContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitDeclaratorid(CPP14Parser::DeclaratoridContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitThetypeid(CPP14Parser::ThetypeidContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitAbstractdeclarator(CPP14Parser::AbstractdeclaratorContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitPtrabstractdeclarator(CPP14Parser::PtrabstractdeclaratorContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitNoptrabstractdeclarator(CPP14Parser::NoptrabstractdeclaratorContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitAbstractpackdeclarator(CPP14Parser::AbstractpackdeclaratorContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitNoptrabstractpackdeclarator(CPP14Parser::NoptrabstractpackdeclaratorContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitParameterdeclarationclause(CPP14Parser::ParameterdeclarationclauseContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitParameterdeclarationlist(CPP14Parser::ParameterdeclarationlistContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitParameterdeclaration(CPP14Parser::ParameterdeclarationContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitFunctiondefinition(CPP14Parser::FunctiondefinitionContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitFunctionbody(CPP14Parser::FunctionbodyContext* ctx) override;

		virtual antlrcpp::Any visitInitializer(CPP14Parser::InitializerContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitBraceorequalinitializer(CPP14Parser::BraceorequalinitializerContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitInitializerclause(CPP14Parser::InitializerclauseContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitInitializerlist(CPP14Parser::InitializerlistContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitBracedinitlist(CPP14Parser::BracedinitlistContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitClassname(CPP14Parser::ClassnameContext* ctx) override;

		virtual antlrcpp::Any visitClassspecifier(CPP14Parser::ClassspecifierContext* ctx) override;

		virtual antlrcpp::Any visitClasshead(CPP14Parser::ClassheadContext* ctx) override;

		virtual antlrcpp::Any visitClassheadname(CPP14Parser::ClassheadnameContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitClassvirtspecifier(CPP14Parser::ClassvirtspecifierContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitClasskey(CPP14Parser::ClasskeyContext* ctx) override;

		virtual antlrcpp::Any visitMemberspecification(CPP14Parser::MemberspecificationContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitMemberdeclaration(CPP14Parser::MemberdeclarationContext* ctx) override;

		virtual antlrcpp::Any visitMemberdeclaratorlist(CPP14Parser::MemberdeclaratorlistContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitMemberdeclarator(CPP14Parser::MemberdeclaratorContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitVirtspecifierseq(CPP14Parser::VirtspecifierseqContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitVirtspecifier(CPP14Parser::VirtspecifierContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitPurespecifier(CPP14Parser::PurespecifierContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitBaseclause(CPP14Parser::BaseclauseContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitBasespecifierlist(CPP14Parser::BasespecifierlistContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitBasespecifier(CPP14Parser::BasespecifierContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitClassordecltype(CPP14Parser::ClassordecltypeContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitBasetypespecifier(CPP14Parser::BasetypespecifierContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitAccessspecifier(CPP14Parser::AccessspecifierContext* ctx) override;

		virtual antlrcpp::Any visitConversionfunctionid(CPP14Parser::ConversionfunctionidContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitConversiontypeid(CPP14Parser::ConversiontypeidContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitConversiondeclarator(CPP14Parser::ConversiondeclaratorContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitCtorinitializer(CPP14Parser::CtorinitializerContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitMeminitializerlist(CPP14Parser::MeminitializerlistContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitMeminitializer(CPP14Parser::MeminitializerContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitMeminitializerid(CPP14Parser::MeminitializeridContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitOperatorfunctionid(CPP14Parser::OperatorfunctionidContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitLiteraloperatorid(CPP14Parser::LiteraloperatoridContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitTemplatedeclaration(CPP14Parser::TemplatedeclarationContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitTemplateparameterlist(CPP14Parser::TemplateparameterlistContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitTemplateparameter(CPP14Parser::TemplateparameterContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitTypeparameter(CPP14Parser::TypeparameterContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitSimpletemplateid(CPP14Parser::SimpletemplateidContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitTemplateid(CPP14Parser::TemplateidContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitTemplatename(CPP14Parser::TemplatenameContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitTemplateargumentlist(CPP14Parser::TemplateargumentlistContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitTemplateargument(CPP14Parser::TemplateargumentContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitTypenamespecifier(CPP14Parser::TypenamespecifierContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitExplicitinstantiation(CPP14Parser::ExplicitinstantiationContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitExplicitspecialization(CPP14Parser::ExplicitspecializationContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitTryblock(CPP14Parser::TryblockContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitFunctiontryblock(CPP14Parser::FunctiontryblockContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitHandlerseq(CPP14Parser::HandlerseqContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitHandler(CPP14Parser::HandlerContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitExceptiondeclaration(CPP14Parser::ExceptiondeclarationContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitThrowexpression(CPP14Parser::ThrowexpressionContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitExceptionspecification(CPP14Parser::ExceptionspecificationContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitDynamicexceptionspecification(CPP14Parser::DynamicexceptionspecificationContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitTypeidlist(CPP14Parser::TypeidlistContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitNoexceptspecification(CPP14Parser::NoexceptspecificationContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitTheoperator(CPP14Parser::TheoperatorContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitLiteral(CPP14Parser::LiteralContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitBooleanliteral(CPP14Parser::BooleanliteralContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitPointerliteral(CPP14Parser::PointerliteralContext* ctx) override {
			return visitChildren(ctx);
		}

		virtual antlrcpp::Any visitUserdefinedliteral(CPP14Parser::UserdefinedliteralContext* ctx) override {
			return visitChildren(ctx);
		}
	};

}
