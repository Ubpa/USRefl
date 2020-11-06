#pragma once

#include "Meta.h"

namespace Ubpa::USRefl {
	class TypeInfoGenerator {
	public:
		enum class ConstMode {
			Constepxr,
			Const,
			NonConst
		};
		
		struct Config {
			bool nonNamespaceAttrNameWithoutQuotation{ false };
			bool namespaceAttrNameWithQuotation{ false };
			bool isAttrValueToFunction{ false };
			
			bool isInitializerAsAttr{ true };
			std::string_view ns_initializer = "";
			std::string_view name_initializer = "__initializer";
			bool isInitializerToFunction{ true };
			
			std::string_view name_constructor = "__constructor";
			std::string_view name_destructor = "__destructor";
			
			bool generateDefaultFunctions{ true };
			std::string_view ns_default_functions = "";
			std::string_view name_default_functions = "__default_functions";
			ConstMode attrListConstMode{ ConstMode::Constepxr };
			ConstMode fieldListConstMode{ ConstMode::Constepxr };
		};

		TypeInfoGenerator(Config config = Config{}) : config{ std::move(config) } {}

		std::string Generate(const std::vector<TypeMeta>& typeMetas);
	private:
		Config config;
	};
}
