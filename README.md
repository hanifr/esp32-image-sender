
frozen_inference_graph.pb is a file containing the trained TensorFlow model graph that is used for making predictions/inferences.

Some key points about frozen_inference_graph.pb:

It contains a serialized GraphDef protocol buffer with the TensorFlow graph. This graph has ops and tensors needed to run inference.
The graph has been "frozen" - meaning the variables are converted to constant values stored in the graph. Freezing makes the graph portable for deployment.
It does not contain graph building information or training ops. Just the math ops for inference.
This file is created via an export process from a trained TensorFlow model. It optimizes the graph for inference.
Using a frozen graph can be faster for inference compared to using saved model files.
So in summary, frozen_inference_graph.pb contains a ready-to-use model graph that has been optimized to efficiently run inferences/predictions on new data.
Loading and running it with a TensorFlow session enables feeding in new data and getting outputs without needing to rebuild or retrain the model.