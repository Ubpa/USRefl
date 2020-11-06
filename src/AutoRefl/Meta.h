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
		std::string GenerateValue(const std::string& type, bool toFunction) const;
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
		std::string initializer; // {expression}
		
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
		std::vector<Attr> attrs;
		std::vector<DeclSpecifier> declSpecifiers;
		std::vector<std::string> pointerOperators; // *, &, &&
		std::string name;
		std::string initializer; // {expression}
		std::vector<Parameter> parameters;
		std::vector<std::string> qualifiers; // const, volatile, &, &&
		bool isTemplate{ false };

		void Clear();
		bool IsStaticConstexpr() const;
		// declSpecifiers + pointerOperators
		std::string GenerateFieldType() const;
		// typeSpecifier + pointerOperators
		std::string GenerateSimpleFieldType() const;
		// Mode::Function && contains 'static'
		bool IsMemberFunction() const;
		std::string GenerateFunctionType(std::string_view obj) const;
		std::string GenerateInitFunction() const;
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

		// namespaces + name + template
		std::string GenerateFullName() const;
		// templateParameter_0, templateParameter_1, ...,  templateParameter_N
		std::string GenerateTemplateList() const;
		std::vector<size_t> GetPublicBaseIndices() const;
		bool IsOverloaded(std::string_view name) const;
	};
}
