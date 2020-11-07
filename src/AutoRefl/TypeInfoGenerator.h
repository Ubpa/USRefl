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

			std::string_view nameof_namespace = _USRefl::nameof_namespace;

			bool isInitializerAsAttr{ true };

			std::string_view nameof_initializer = _USRefl::nameof_initializer;
			bool isInitializerToFunction{ true };

			std::string_view nameof_constructor = _USRefl::nameof_constructor;
			std::string_view nameof_destructor = _USRefl::nameof_destructor;
			
			bool generateDefaultFunctions{ true };
			std::string_view nameof_default_functions = _USRefl::nameof_default_functions;
			ConstMode attrListConstMode{ ConstMode::Constepxr };
			ConstMode fieldListConstMode{ ConstMode::Constepxr };
		};

		TypeInfoGenerator(Config config = Config{}) : config{ std::move(config) } {}

		std::string Generate(const std::vector<TypeMeta>& typeMetas);
	private:
		Config config;
	};
}
