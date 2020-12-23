#pragma once

#include <vector>
#include <string_view>
#include <string>

namespace Ubpa::USRefl {
	struct Attr {
		std::string ns;
		std::string name;
		std::string value;

		// ns::name
		// name
		std::string GenerateName(bool withoutQuatation) const;
		std::string GenerateValue(bool toFunction) const;
		std::string GenerateValue(const std::string& type) const;
	};

	enum class AccessSpecifier {
		PUBLIC,
		PROTECTED,
		PRIVATE,
		DEFAULT
	};
	
	using DeclSpecifier = std::string;

	struct Parameter {
		//std::vector<Attr> attrs;
		bool isPacked{ false };
		std::string type; // typename, class, ...
		std::string name;
		std::string initializer; // expression or {expression}
		
		// type[...]
		std::string GenerateTypeName() const;
		// type[...] name
		std::string GenerateParameterName() const;
		// name[...]
		std::string GenerateArgumentName() const;
	};
	
	struct Field {
		enum class Mode {
			Variable,
			Function,
			Value // enum, static constepxr
		};
		Mode mode{ Mode::Variable };
		AccessSpecifier accessSpecifier{ AccessSpecifier::PRIVATE };
		std::vector<Attr> attrs;
		std::vector<DeclSpecifier> declSpecifiers;
		std::vector<std::string> pointerOperators; // *, &, &&
		std::string name;
		std::string initializer; // expression or {expression}
		std::vector<Parameter> parameters;
		std::vector<std::string> qualifiers; // const, volatile, &, &&
		bool isTemplate{ false };

		void Clear();
		bool IsStaticConstexprVariable() const;
		// declSpecifiers + pointerOperators
		std::string GenerateFieldType() const;
		// typeSpecifier + pointerOperators
		std::string GenerateSimpleFieldType() const;
		// Mode::Function && contains 'static' && !friend
		bool IsMemberFunction() const;
		bool IsFriendFunction() const;
		bool IsDeletedFunction() const;
		// arg_type0, arg_type1, ..., arg_typeN
		std::string GenerateParamTypeList() const;
		// arg_type0, arg_type1, ..., arg_type(num-1)
		std::string GenerateParamTypeList(std::size_t num) const;
		std::string GenerateFunctionType(std::string_view obj) const;
		std::string GenerateInitFunction() const;
		std::size_t GetDefaultParameterNum() const;
		// arg_type0 arg_name0, arg_type1 arg_name1, ..., arg_type(num-1) arg_name(num-1)
		std::string GenerateNamedParameterList(std::size_t num) const;
		// std::forward<arg_type0>(arg_name0), std::forward<arg_type0>(arg_name1), ..., std::forward<arg_type0>(arg_name(num-1))
		std::string GenerateForwardArgumentList(std::size_t num) const;
		std::string GenerateQualifiers() const;
	};

	struct Base {
		// Base<name[, true]>[...]
		std::string GenerateText() const;

		AccessSpecifier accessSpecifier{ AccessSpecifier::DEFAULT };
		std::string name;
		bool isVirtual{ false };
		bool isPacked{ false };
	};

	struct TypeMeta {
		enum class Mode {
			Class,
			Struct,
			Enum
		};

		Mode mode{ Mode::Class };
		std::vector<std::string> namespaces;
		std::vector<Parameter> templateParameters;
		std::vector<Attr> attrs;
		std::string name;
		std::vector<Base> bases;
		std::vector<Field> fields;

		bool IsTemplateType() const noexcept { return !templateParameters.empty(); }

		// namespaces + name
		std::string GenerateNsName() const;
		// namespaces + name + template
		std::string GenerateFullName() const;
		// templateParameter_0, templateParameter_1, ...,  templateParameter_N
		std::string GenerateTemplateList() const;
		std::vector<std::size_t> GetPublicBaseIndices() const;
		bool IsOverloaded(std::string_view name) const;
		// public, non-friend, non-delete
		bool HaveAnyOutputField() const;
	};
}
