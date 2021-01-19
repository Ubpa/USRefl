#pragma once

#include "Meta.h"
#include <USRefl/config.h>

namespace Ubpa::USRefl {
	class TypeInfoGenerator {
	public:
		enum class ConstMode {
			Constepxr,
			Const,
			NonConst
		};
		
		struct Config {
			bool nonNamespaceNameWithoutQuotation{ false };
			bool namespaceNameWithQuotation{ false };
			bool isAttrValueToFunction{ false };

			std::string_view nameof_namespace = UMeta::nameof_namespace;

			bool isInitializerAsAttr{ false };

			std::string_view nameof_initializer = UMeta::nameof_initializer;

			std::string_view nameof_constructor = UMeta::nameof_constructor;
			std::string_view nameof_destructor = UMeta::nameof_destructor;
			
			bool generateDefaultFunctions{ true };
			std::string_view nameof_default_functions = UMeta::nameof_default_functions;
			ConstMode attrListConstMode{ ConstMode::Constepxr };
			ConstMode fieldListConstMode{ ConstMode::Constepxr };
		};

		TypeInfoGenerator() = default;
		TypeInfoGenerator(Config config ) : config{ std::move(config) } {}

		std::string Generate(const std::vector<TypeMeta>& typeMetas);
	private:
		Config config;
	};
}
