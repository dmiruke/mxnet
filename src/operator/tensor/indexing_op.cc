/*!
 * Copyright (c) 2017 by Contributors
 * \file indexing_op.cc
 * \brief
 * \author Siyi Li, Chi Zhang
*/

#include "./indexing_op.h"
namespace mxnet {
namespace op {
DMLC_REGISTER_PARAMETER(EmbeddingParam);

NNVM_REGISTER_OP(Embedding)
.MXNET_DESCRIBE("Map integer index to vector representations (embeddings)."
                " Those embeddings are learnable parameters. For a input of shape"
                " (d1, ..., dK), the output shape is (d1, ..., dK, output_dim)."
                " All the input values should be integers in the range [0, input_dim).")
.set_num_inputs(2)
.set_num_outputs(1)
.set_attr_parser(ParamParser<EmbeddingParam>)
.set_attr<nnvm::FListInputNames>("FListInputNames",
  [](const NodeAttrs& attrs) {
    return std::vector<std::string>{"data", "weight"};
  })
.set_attr<nnvm::FInferShape>("FInferShape", EmbeddingOpShape)
.set_attr<nnvm::FInferType>("FInferType", EmbeddingOpType)
.set_attr<FResourceRequest>("FResourceRequest",
  [](const NodeAttrs& attrs) {
    return std::vector<ResourceRequest>{ResourceRequest::kTempSpace};
  })
.set_attr<FCompute>("FCompute<cpu>", TakeOpForward<cpu>)
.set_attr<nnvm::FGradient>("FGradient",
  [](const nnvm::NodePtr& n,  const std::vector<nnvm::NodeEntry>& ograds) {
    std::vector<nnvm::NodeEntry> heads(ograds.begin(), ograds.end());
    heads.push_back(n->inputs[0]);
    return MakeGradNode("_backward_take", n, heads, n->attrs.dict);
  })
.add_argument("data", "Symbol", "Input data to the EmbeddingOp.")
.add_argument("weight", "Symbol", "Embedding weight matrix.")
.add_arguments(EmbeddingParam::__FIELDS__());

DMLC_REGISTER_PARAMETER(TakeParam);

NNVM_REGISTER_OP(take)
.MXNET_DESCRIBE("Take row vectors from a 2D matrix according to the indices"
                " For an input of index with shape (d1, ..., dK), the output"
                " shape is (d1, ..., dK, row_vector_length).All the input"
                " values should be integers in the range"
                " [0, column_vector_length).")
.set_num_inputs(2)
.set_num_outputs(1)
.set_attr_parser(ParamParser<TakeParam>)
.set_attr<nnvm::FListInputNames>("FListInputNames",
  [](const NodeAttrs& attrs) {
    return std::vector<std::string>{"idx", "data"};
  })
.set_attr<nnvm::FInferShape>("FInferShape", TakeOpShape)
.set_attr<nnvm::FInferType>("FInferType", TakeOpType)
.set_attr<FResourceRequest>("FResourceRequest",
  [](const NodeAttrs& attrs) {
    return std::vector<ResourceRequest>{ResourceRequest::kTempSpace};
  })
.set_attr<FCompute>("FCompute<cpu>", TakeOpForward<cpu>)
.set_attr<nnvm::FGradient>("FGradient",
  [](const nnvm::NodePtr& n,  const std::vector<nnvm::NodeEntry>& ograds) {
    std::vector<nnvm::NodeEntry> heads(ograds.begin(), ograds.end());
    heads.push_back(n->inputs[0]);
    return MakeGradNode("_backward_take", n, heads, n->attrs.dict);
  })
.add_argument("idx", "Symbol", "Tensor that records the indices to be taken in data.")
.add_argument("data", "Symbol", "2D matrix to be taken.")
.add_arguments(TakeParam::__FIELDS__());

NNVM_REGISTER_OP(_backward_take)
.set_num_inputs(2)
.set_num_outputs(2)
.set_attr<FResourceRequest>("FResourceRequest",
  [](const NodeAttrs& attrs) {
    return std::vector<ResourceRequest>{ResourceRequest::kTempSpace};
  })
.set_attr<nnvm::TIsBackward>("TIsBackward", true)
.set_attr<FCompute>("FCompute<cpu>", TakeOpBackward<cpu>);
}  // namespace op
}  // namespace mxnet
