#include "Meta.h"

#include <cassert>

using namespace Ubpa::USRefl;

std::string Attr::GenerateName(bool withoutQuatation) const {
	auto rst = ns.empty() ? name : ns + "::" + name;
	if (!withoutQuatation)
		rst = "\"" + rst + "\"";
	
	return "TSTR(" + rst + ")";
}

std::string Attr::GenerateValue(bool toFunction) const {
	if(toFunction)
		return value.empty() ? "[]{}" : "[]{ return " + value + "; }";
	return value.empty() ? "0" : value;
}

std::string Attr::GenerateValue(const std::string& type) const {
	return "[]()->" + type + "{ return " + (value.empty() ? "{}" : value) + "; }";
}

std::string Parameter::GenerateTypeName() const {
	std::string rst = type;
	if (isPacked)
		rst += "...";
	return rst;
}

std::string Parameter::GenerateParameterName() const {
	std::string rst = GenerateTypeName();
	if (isPacked)
		rst += "...";
	rst += " ";
	rst += name;
	return rst;
}

std::string Parameter::GenerateArgumentName() const {
	std::string rst = name;
	if (isPacked)
		rst += "...";
	return rst;
}

std::string TypeMeta::GenerateNsName() const {
	std::string rst;
	for (const auto& ns : namespaces) {
		rst += ns;
		rst += "::";
	}

	rst += name;

	return rst;
}

std::string TypeMeta::GenerateFullName() const {
	std::string rst = GenerateNsName();

	if (!IsTemplateType())
		return rst;

	std::size_t idx = 0;
	rst += "<";
	for (std::size_t i = 0; i < templateParameters.size(); i++) {
		const auto& ele = templateParameters[i];
		if (ele.name.empty()) {
			rst += "_";
			rst += std::to_string(idx);
			idx++;
		}
		else
			rst += ele.name;
		if (ele.isPacked)
			rst += "...";

		if (i != templateParameters.size() - 1)
			rst += ", ";
	}
	rst += ">";

	return rst;
}

std::string TypeMeta::GenerateTemplateList() const {
	std::string rst;

	std::size_t idx = 0;
	for (std::size_t i = 0; i < templateParameters.size(); i++) {
		const auto& ele = templateParameters[i];
		rst += ele.type;
		if (ele.isPacked)
			rst += "...";
		rst += " ";
		if (ele.name.empty()) {
			rst += "_";
			rst += std::to_string(idx);
			idx++;
		}
		else
			rst += ele.name;
		if (i != templateParameters.size() - 1)
			rst += ", ";
	}
	
	return rst;
}

std::vector<std::size_t> TypeMeta::GetPublicBaseIndices() const {
	std::vector<std::size_t> rst;
	for(std::size_t i=0;i<bases.size();i++) {
		const auto& base = bases[i];
		if
		(
			base.accessSpecifier == AccessSpecifier::DEFAULT && mode == Mode::Struct
			|| base.accessSpecifier == AccessSpecifier::PUBLIC
		)
			rst.push_back(i);
	}
	return rst;
}

std::string Base::GenerateText() const {
	std::string rst = "Base<";
	
	rst += name;
	
	if (isVirtual)
		rst += ", true";
	rst += ">";

	if (isPacked)
		rst += "...";

	return rst;
}

void Field::Clear() {
	mode = Mode::Variable;
	attrs.clear();
	declSpecifiers.clear();
	pointerOperators.clear();
	name.clear();
	parameters.clear();
	qualifiers.clear();
}

bool Field::IsStaticConstexprVariable() const {
	bool containsStatic = false;
	bool containsConstexpr = false;
	for(const auto& declSpecifier : declSpecifiers) {
		if (declSpecifier == "static")
			containsStatic = true;
		else if (declSpecifier == "constexpr")
			containsConstexpr = true;
	}
	return mode == Mode::Variable && containsStatic && containsConstexpr;
}

std::string Field::GenerateFieldType() const {
	std::string rst;
	
	for (std::size_t i = 0; i < declSpecifiers.size(); i++) {
		rst += declSpecifiers[i];
		if (i != declSpecifiers.size() - 1)
			rst += " ";
	}
	
	if(!pointerOperators.empty()) {
		if (!rst.empty())
			rst += " ";
		for (std::size_t i = 0; i < pointerOperators.size(); i++) {
			rst += pointerOperators[i];
			if (i != pointerOperators.size() - 1)
				rst += " ";
		}
	}
	
	if (rst.empty())
		return "int"; // default

	return rst;
}

std::string Field::GenerateSimpleFieldType() const {
	std::string rst;

	std::vector<std::size_t> filterdIndice;
	for (std::size_t i = 0; i < declSpecifiers.size(); i++) {
		const auto& declSpecifier = declSpecifiers[i];
		if
		(
			// storage class specifier
			   declSpecifier != "register"
			&& declSpecifier != "static"
			&& declSpecifier != "thread_local"
			&& declSpecifier != "extern"
			&& declSpecifier != "mutable"

			// function specifier
			&& declSpecifier != "inline"
			&& declSpecifier != "virtual"
			&& declSpecifier != "explicit"

			// others
			&& declSpecifier != "friend"
			&& declSpecifier != "typedef"
			&& declSpecifier != "constexpr"
		)
			filterdIndice.push_back(i);
	}

	for (std::size_t i = 0; i < filterdIndice.size(); i++) {
		rst += declSpecifiers[filterdIndice[i]];
		if (i != filterdIndice.size() - 1)
			rst += " ";
	}

	if (!pointerOperators.empty()) {
		if (!rst.empty())
			rst += " ";
		for (std::size_t i = 0; i < pointerOperators.size(); i++) {
			rst += pointerOperators[i];
			if (i != pointerOperators.size() - 1)
				rst += " ";
		}
	}

	if (rst.empty())
		return "int"; // default

	return rst;
}

bool Field::IsMemberFunction() const {
	if (mode != Mode::Function)
		return false;
	
	bool containsStatic = false;
	for(const auto& declSpecifier : declSpecifiers) {
		if (declSpecifier == "static")
			containsStatic = true;
	}
	bool containsFriend = false;
	for (const auto& declSpecifier : declSpecifiers) {
		if (declSpecifier == "friend")
			containsFriend = true;
	}
	
	return !containsStatic && !containsFriend;
}

bool Field::IsFriendFunction() const {
	if (mode != Mode::Function)
		return false;

	for (const auto& declSpecifier : declSpecifiers) {
		if (declSpecifier == "friend")
			return true;
	}
	return false;
}

bool Field::IsDeletedFunction() const {
	return mode == Mode::Function && initializer == "delete";
}

std::string Field::GenerateParamTypeList() const {
	return GenerateParamTypeList(parameters.size());
}

std::string Field::GenerateParamTypeList(std::size_t num) const {
	assert(num <= parameters.size());
	std::string rst;
	for (std::size_t i = 0; i < num; i++) {
		rst += parameters[i].GenerateTypeName();
		if (i != num - 1)
			rst += ", ";
	}
	return rst;
}

std::size_t Field::GetDefaultParameterNum() const {
	if (mode != Mode::Function)
		return 0;
	for(std::size_t i=0;i<parameters.size();i++) {
		if(!parameters[i].initializer.empty())
			return parameters.size() - i;
	}
	return 0;
}

std::string Field::GenerateNamedParameterList(std::size_t num) const {
	assert(num <= parameters.size());
	std::string rst;
	std::size_t idx = 0;
	for (std::size_t i = 0; i < num; i++) {
		rst += parameters[i].GenerateTypeName();
		if (parameters[i].isPacked)
			rst += "...";
		rst += " ";
		if (parameters[i].name.empty()) {
			rst += "_" + std::to_string(idx);
			idx++;
		}
		else
			rst += parameters[i].name;
		if (i != num - 1)
			rst += ", ";
	}
	return rst;
}

std::string Field::GenerateForwardArgumentList(std::size_t num) const {
	assert(num <= parameters.size());
	std::string rst;
	std::size_t idx = 0;
	for (std::size_t i = 0; i < num; i++) {
		rst += "std::forward<";
		rst += parameters[i].GenerateTypeName();
		rst += ">(";
		if(parameters[i].name.empty()) {
			std::string name = "_" + std::to_string(idx);
			idx++;
			rst += name;
		}
		else
			rst += parameters[i].name;
		rst += ")";
		if (parameters[i].isPacked)
			rst += "...";
		if (i != num - 1)
			rst += ", ";
	}
	return rst;
}

std::string Field::GenerateQualifiers() const {
	std::string rst;
	for (std::size_t i = 0; i < qualifiers.size(); i++) {
		rst += qualifiers[i];
		if (i != qualifiers.size() - 1)
			rst += " ";
	}
	return rst;
}

std::string Field::GenerateFunctionType(std::string_view obj) const {
	assert(mode == Mode::Function);
	
	std::string rst;
	rst += GenerateSimpleFieldType();
	if (IsMemberFunction()) {
		rst += "(";
		rst += obj;
		rst += "::*)";
	}
	rst += "(";
	rst += GenerateParamTypeList();
	rst += ")";
	for (std::size_t i = 0; i < qualifiers.size(); i++) {
		rst += qualifiers[i];
		if (i != qualifiers.size() - 1)
			rst += " ";
	}
	return rst;
}

std::string Field::GenerateInitFunction() const {
	return "[]{ return " + GenerateSimpleFieldType() + (!initializer.empty() ? initializer : "{}") + "; }";
}

bool TypeMeta::IsOverloaded(std::string_view name) const {
	std::size_t cnt = 0;
	for(const auto& field : fields) {
		if (field.name == name && !field.IsFriendFunction())
			cnt++;
	}
	return cnt > 1;
}

bool TypeMeta::HaveAnyOutputField() const {
	for (const auto& field : fields) {
		if (field.accessSpecifier == AccessSpecifier::PUBLIC && !field.IsFriendFunction() && !field.IsDeletedFunction())
			return true;
	}
	return false;
}
