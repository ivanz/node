// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/compiler/js-operator.h"
#include "src/compiler/opcodes.h"
#include "src/compiler/operator.h"
#include "src/compiler/operator-properties.h"
#include "test/unittests/test-utils.h"

namespace v8 {
namespace internal {
namespace compiler {

// -----------------------------------------------------------------------------
// Shared operators.

namespace {

struct SharedOperator {
  const Operator* (JSOperatorBuilder::*constructor)();
  IrOpcode::Value opcode;
  Operator::Properties properties;
  int value_input_count;
  int frame_state_input_count;
  int effect_input_count;
  int control_input_count;
  int value_output_count;
  int effect_output_count;
  int control_output_count;
};

const SharedOperator kSharedOperators[] = {
#define SHARED(Name, properties, value_input_count, frame_state_input_count, \
               effect_input_count, control_input_count, value_output_count,  \
               effect_output_count, control_output_count)                    \
  {                                                                          \
    &JSOperatorBuilder::Name, IrOpcode::kJS##Name, properties,               \
        value_input_count, frame_state_input_count, effect_input_count,      \
        control_input_count, value_output_count, effect_output_count,        \
        control_output_count                                                 \
  }
    SHARED(Equal, Operator::kNoProperties, 2, 1, 1, 1, 1, 1, 2),
    SHARED(NotEqual, Operator::kNoProperties, 2, 1, 1, 1, 1, 1, 2),
    SHARED(StrictEqual, Operator::kNoThrow, 2, 0, 1, 1, 1, 1, 0),
    SHARED(StrictNotEqual, Operator::kNoThrow, 2, 0, 1, 1, 1, 1, 0),
    SHARED(LessThan, Operator::kNoProperties, 2, 2, 1, 1, 1, 1, 2),
    SHARED(GreaterThan, Operator::kNoProperties, 2, 2, 1, 1, 1, 1, 2),
    SHARED(LessThanOrEqual, Operator::kNoProperties, 2, 2, 1, 1, 1, 1, 2),
    SHARED(GreaterThanOrEqual, Operator::kNoProperties, 2, 2, 1, 1, 1, 1, 2),
    SHARED(ToNumber, Operator::kNoProperties, 1, 1, 1, 1, 1, 1, 2),
    SHARED(ToString, Operator::kNoProperties, 1, 1, 1, 1, 1, 1, 2),
    SHARED(ToName, Operator::kNoProperties, 1, 1, 1, 1, 1, 1, 2),
    SHARED(ToObject, Operator::kNoProperties, 1, 1, 1, 1, 1, 1, 2),
    SHARED(Yield, Operator::kNoProperties, 1, 0, 1, 1, 1, 1, 2),
    SHARED(Create, Operator::kEliminatable, 2, 1, 1, 0, 1, 1, 0),
    SHARED(HasProperty, Operator::kNoProperties, 2, 1, 1, 1, 1, 1, 2),
    SHARED(TypeOf, Operator::kEliminatable, 1, 0, 1, 0, 1, 1, 0),
    SHARED(InstanceOf, Operator::kNoProperties, 2, 1, 1, 1, 1, 1, 2),
    SHARED(CreateWithContext, Operator::kNoProperties, 2, 0, 1, 1, 1, 1, 2),
    SHARED(CreateModuleContext, Operator::kNoProperties, 2, 0, 1, 1, 1, 1, 2),
#undef SHARED
};


std::ostream& operator<<(std::ostream& os, const SharedOperator& sop) {
  return os << IrOpcode::Mnemonic(sop.opcode);
}

}  // namespace


class JSSharedOperatorTest
    : public TestWithZone,
      public ::testing::WithParamInterface<SharedOperator> {};


TEST_P(JSSharedOperatorTest, InstancesAreGloballyShared) {
  const SharedOperator& sop = GetParam();
  JSOperatorBuilder javascript1(zone());
  JSOperatorBuilder javascript2(zone());
  EXPECT_EQ((javascript1.*sop.constructor)(), (javascript2.*sop.constructor)());
}


TEST_P(JSSharedOperatorTest, NumberOfInputsAndOutputs) {
  JSOperatorBuilder javascript(zone());
  const SharedOperator& sop = GetParam();
  const Operator* op = (javascript.*sop.constructor)();

  const int context_input_count = 1;
  EXPECT_EQ(sop.value_input_count, op->ValueInputCount());
  EXPECT_EQ(context_input_count, OperatorProperties::GetContextInputCount(op));
  EXPECT_EQ(sop.frame_state_input_count,
            OperatorProperties::GetFrameStateInputCount(op));
  EXPECT_EQ(sop.effect_input_count, op->EffectInputCount());
  EXPECT_EQ(sop.control_input_count, op->ControlInputCount());
  EXPECT_EQ(sop.value_input_count + context_input_count +
                sop.frame_state_input_count + sop.effect_input_count +
                sop.control_input_count,
            OperatorProperties::GetTotalInputCount(op));

  EXPECT_EQ(sop.value_output_count, op->ValueOutputCount());
  EXPECT_EQ(sop.effect_output_count, op->EffectOutputCount());
  EXPECT_EQ(sop.control_output_count, op->ControlOutputCount());
}


TEST_P(JSSharedOperatorTest, OpcodeIsCorrect) {
  JSOperatorBuilder javascript(zone());
  const SharedOperator& sop = GetParam();
  const Operator* op = (javascript.*sop.constructor)();
  EXPECT_EQ(sop.opcode, op->opcode());
}


TEST_P(JSSharedOperatorTest, Properties) {
  JSOperatorBuilder javascript(zone());
  const SharedOperator& sop = GetParam();
  const Operator* op = (javascript.*sop.constructor)();
  EXPECT_EQ(sop.properties, op->properties());
}


INSTANTIATE_TEST_CASE_P(JSOperatorTest, JSSharedOperatorTest,
                        ::testing::ValuesIn(kSharedOperators));

}  // namespace compiler
}  // namespace internal
}  // namespace v8
