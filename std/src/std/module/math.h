#pragma once
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include "std/module.h"
#include "Runtime/Service/BridgeBuilder.h"

namespace ska {
    namespace lang {
        // Mathematics : Math = import "bind:std.native.math" (see the "std.math" script template)
        template <class Interpreter>
        class MathModule : public Module<Interpreter> {
        public:
            MathModule(ModuleConfiguration<Interpreter>& config) :
                Module<Interpreter> {config, "std.native.math", "std:std.math"},
                m_builder(BridgeBuilder<Interpreter> { Module<Interpreter>::m_bridge }) {

                m_builder.bindField("abs", [](std::vector<NodeValue> params) {
                    const auto value = params[0].nodeval<long>();
                    return NodeValue{ value < 0 ? -value : value };
                });
                m_builder.bindField("min", [](std::vector<NodeValue> params) {
                    return NodeValue{ std::min(params[0].nodeval<long>(), params[1].nodeval<long>()) };
                });
                m_builder.bindField("max", [](std::vector<NodeValue> params) {
                    return NodeValue{ std::max(params[0].nodeval<long>(), params[1].nodeval<long>()) };
                });
                m_builder.bindField("clamp", [](std::vector<NodeValue> params) {
                    const auto low = params[1].nodeval<long>();
                    const auto high = params[2].nodeval<long>();
                    if (low > high) {
                        throw std::runtime_error("clamp : the low limit is greater than the high one");
                    }
                    return NodeValue{ std::clamp(params[0].nodeval<long>(), low, high) };
                });

                m_builder.bindField("absFloat", [](std::vector<NodeValue> params) {
                    return NodeValue{ std::fabs(params[0].nodeval<double>()) };
                });
                m_builder.bindField("minFloat", [](std::vector<NodeValue> params) {
                    return NodeValue{ std::min(params[0].nodeval<double>(), params[1].nodeval<double>()) };
                });
                m_builder.bindField("maxFloat", [](std::vector<NodeValue> params) {
                    return NodeValue{ std::max(params[0].nodeval<double>(), params[1].nodeval<double>()) };
                });
                m_builder.bindField("sqrt", [](std::vector<NodeValue> params) {
                    const auto value = params[0].nodeval<double>();
                    if (value < 0) {
                        throw std::runtime_error("square root of a negative number");
                    }
                    return NodeValue{ std::sqrt(value) };
                });
                m_builder.bindField("pow", [](std::vector<NodeValue> params) {
                    return NodeValue{ std::pow(params[0].nodeval<double>(), params[1].nodeval<double>()) };
                });

                m_builder.bindField("floor", [](std::vector<NodeValue> params) {
                    return NodeValue{ static_cast<long>(std::floor(params[0].nodeval<double>())) };
                });
                m_builder.bindField("ceil", [](std::vector<NodeValue> params) {
                    return NodeValue{ static_cast<long>(std::ceil(params[0].nodeval<double>())) };
                });
                m_builder.bindField("round", [](std::vector<NodeValue> params) {
                    return NodeValue{ static_cast<long>(std::lround(params[0].nodeval<double>())) };
                });

                m_builder.generate();
            }
            ~MathModule() override = default;
        private:
            BridgeBuilder<Interpreter> m_builder;
        };
    }
}
