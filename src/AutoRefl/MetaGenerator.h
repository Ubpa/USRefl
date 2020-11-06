#pragma once

#include "Meta.h"

#include <memory>

namespace Ubpa::USRefl {
    class MetaGenerator {
    public:
		MetaGenerator();
		~MetaGenerator();
		MetaGenerator(MetaGenerator&&) noexcept;
        MetaGenerator& operator=(MetaGenerator&&) noexcept;

        std::vector<TypeMeta> Parse(std::string_view cppCode);
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
}
