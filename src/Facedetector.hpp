/*
 * This file is part of project link.developers/ld-node-facedetector.
 * It is copyrighted by the contributors recorded in the version control history of the file,
 * available from its original location https://gitlab.com/link.developers.beta/ld-node-facedetector.
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

template <long num_filters, typename SUBNET> using con5d = 
dlib::con<num_filters, 5, 5, 2, 2, SUBNET>;

template <long num_filters, typename SUBNET> using con5 = 
dlib::con<num_filters, 5, 5, 1, 1, SUBNET>;

template <typename SUBNET> using downsampler = 
dlib::relu<dlib::affine<con5d<32, dlib::relu<dlib::affine<con5d<32, dlib::relu<dlib::affine<con5d<16, SUBNET>>>>>>>>>;

template <typename SUBNET> using rcon5 = 
dlib::relu<dlib::affine<con5<45, SUBNET>>>;

using neuralNetType = 
dlib::loss_mmod<dlib::con<1, 9, 9, 1, 1, rcon5<rcon5<rcon5<downsampler<dlib::input_rgb_image_pyramid<dlib::pyramid_down<6>>>>>>>>;

namespace link_dev
{
	namespace Services 
	{
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
1. http://dlib.net/dnn_mmod_face_detection_ex.cpp.html
2. http://dlib.net/face_detection_ex.cpp.html
3. http://dlib.net/dnn_introduction_ex.cpp.html
4. http://dlib.net/dnn_introduction2_ex.cpp.html
5. https://google.github.io/flatbuffers/flatbuffers_guide_tutorial.html

*/