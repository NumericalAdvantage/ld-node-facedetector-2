/*
 * This file is part of project link.developers/ld-node-facedetector.
 * It is copyrighted by the contributors recorded in the version control history of the file,
 * available from its original location https://gitlab.com/link.developers.beta/ld-node-facedetector-2.
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef FACEDETECTOR_HPP__
#define FACEDETECTOR_HPP__

#include <iostream>
#include <opencv2/highgui.hpp>
#include <dlib/dnn.h>

#include <DRAIVE/Link2/NodeDiscovery.hpp>
#include <DRAIVE/Link2/NodeResources.hpp>
#include <DRAIVE/Link2/SignalHandler.hpp>
#include <DRAIVE/Link2/ConfigurationNode.hpp>
#include <DRAIVE/Link2/OutputPin.hpp>
#include <DRAIVE/Link2/InputPin.hpp>

/*The templated aliases below mention a template type 'SUBNET'.
  This is what is meant by SUBNET:

    +---------------------------------------------------------+
    | loss <-- layer1 <-- layer2 <-- ... <-- layern <-- input |
    +---------------------------------------------------------+
                       ^                            ^
                       \__ subnetwork for layer1 __/
*/

/*Templated alias for a block which will take the subnetwork passed to 
  it and pass it through a convolution layer with num_filters convolutions 
  with a 5x5 filter and 2x2 stride.*/
template <long num_filters, typename SUBNET> using con5d = 
dlib::con<num_filters, 5, 5, 2, 2, SUBNET>;

/*Templated alias for a block which will take the subnetwork passed to 
  it and pass it through a convolution layer with num_filters convolutions 
  with a 5x5 filter and 1x1 stride.*/
template <long num_filters, typename SUBNET> using con5 = 
dlib::con<num_filters, 5, 5, 1, 1, SUBNET>;

/*Templated alias for a block which will take the subnetwork passed to it
  and apply 16 convolutions in the way as defined in the first block above
  which is then passed to an affine layer, the entire subnet following the 
  output of above is then subjected to another 32 convolutions of filter 
  5x5 and stride 2x2, followed by another affine layer and then a ReLU
  followed by another 32 convolutions.  

  Affine layer definition from References[1]:
  "It applies a simple pointwise linear transformation to an input tensor.  
  You can think of it as having two parameter tensors, A and B.  If the 
  input tensor is called INPUT then the output of this layer is:
                   A*INPUT+B
  where all operations are performed element wise and each sample in the
  INPUT tensor is processed separately.*/
template <typename SUBNET> using downsampler = 
dlib::relu<dlib::affine<con5d<32, dlib::relu<dlib::affine<con5d<32, dlib::relu<dlib::affine<con5d<16, SUBNET>>>>>>>>>;

/*Templated alias for a block which will pass the subnetwork passed to it
  through 45 convolutions of filter size 5x5 and strde 2x2 followed 
  by an affine layer fllowed by a ReLU layer.*/
template <typename SUBNET> using rcon5 = 
dlib::relu<dlib::affine<con5<45, SUBNET>>>;

/* Broadly speaking, there are 3 parts to a neural network definition.  
   The loss layer, a bunch of computational layers, and then an input layer. 
   You can see these components in the network definition below.
   The loss layer used is max-margin object detection. You can see that an
   image (type dlib rgb) downsampled with a ration of 6 to 5 forms the input.  
   The middle layers define the computation the network will do to transform the
   input into whatever we want. Here we see 4 layers of the block defined just above 
   being used.*/
using neuralNetType = dlib::loss_mmod<dlib::con<1, 9, 9, 1, 1, rcon5<rcon5<rcon5<downsampler<dlib::input_rgb_image_pyramid<dlib::pyramid_down<6>>>>>>>>;

namespace link_dev
{
	namespace Services 
	{
		/*Uses a neural network (which uses a pre generated model trained on human faces)
		  to check if an incoming frame of a video contains faces and if yes, creates
		  "bounding boxes" around the faces found and publishes both the image and the 
		  bounding boxes.		
		*/
		class FaceDetector 
		{
			DRAIVE::Link2::SignalHandler m_signalHandler;
      DRAIVE::Link2::NodeResources m_nodeResources;
      DRAIVE::Link2::NodeDiscovery m_nodeDiscovery;
      DRAIVE::Link2::OutputPin m_outputPin;
			DRAIVE::Link2::InputPin m_inputPin;
		  bool m_debugActivated = false;
			neuralNetType m_neuralNet;
			std::string m_pathToModel;

			public:
			FaceDetector(DRAIVE::Link2::SignalHandler signalHandler,
                   DRAIVE::Link2::NodeResources nodeResources,
                   DRAIVE::Link2::NodeDiscovery nodeDiscovery,
                   DRAIVE::Link2::OutputPin outputPin,
						       DRAIVE::Link2::InputPin inputPin,
						       bool isdebugMode,
						       std::string pathToModelFile) :
						       m_signalHandler(signalHandler),
                   m_nodeResources(nodeResources),
                   m_nodeDiscovery(nodeDiscovery),
                   m_outputPin(outputPin),
						       m_inputPin(inputPin),
						       m_debugActivated(isdebugMode),
						       m_pathToModel(pathToModelFile)
			{}
			void DetectFaces(const cv::Mat& currentFrame);
			int Run();
		};
	}
}

#endif

/*
References:
1. http://dlib.net/dlib/dnn/layers_abstract.h.html
2. http://dlib.net/dnn_mmod_face_detection_ex.cpp.html
3. http://dlib.net/dlib/dnn/loss_abstract.h.html
4. https://arxiv.org/pdf/1502.00046.pdf
5. http://dlib.net/face_detection_ex.cpp.html
6. http://dlib.net/dnn_introduction_ex.cpp.html
7. http://dlib.net/dnn_introduction2_ex.cpp.html
8. http://blog.dlib.net/2016/10/easily-create-high-quality-object.html
9. https://google.github.io/flatbuffers/flatbuffers_guide_tutorial.html

*/
