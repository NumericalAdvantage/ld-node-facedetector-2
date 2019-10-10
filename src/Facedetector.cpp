/*
 * This file is part of project link.developers/ld-node-facedetector.
 * It is copyrighted by the contributors recorded in the version control history of the file,
 * available from its original location https://gitlab.com/link.developers.beta/ld-node-facedetector.
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "Facedetector.hpp"

#include <dlib/image_transforms.h>
#include <dlib/image_processing.h>
#include <dlib/data_io.h>
#include <dlib/opencv/cv_image.h>

#include <link_dev/Interfaces/OpenCvToImage.h>


int link_dev::Services::FaceDetector::Run() 
{
	dlib::deserialize(m_pathToModel) >> m_neuralNet;

	m_inputPin.addOnDataCallback("l2demand:/videoinput_thruimages", 
	[&](const link_dev::ImageT& receivedImage) 
	{
		DetectFaces(link_dev::Interfaces::ImageToOpenCV(receivedImage));
	});

	while(m_signalHandler.receiveSignal() != LINK2_SIGNAL_INTERRUPT);
	std::cout << "Ending the Run() function." << std::endl;
	return 0;
}

void link_dev::Services::FaceDetector::DetectFaces(cv::Mat& currentFrame) 
{
	try
	{
		/*
		auto result = link_dev::Data::Imaging::Image2D_Plus_Points();
		auto facePositionData = link_dev::Data::Math::List_Tuple_UInt64_UInt64();
		auto rereadOpencvMat = link_dev::Data::Imaging::Image2DInterfaces::ToOpenCV(msg);

		*result.mutable_image() = msg.getPayload();
		*/
		link_dev::Interfaces::
		link_dev::Data::Imaging::Format imgFormat;
		std::vector<dlib::mmod_rect> detFaces;
		dlib::matrix<dlib::rgb_pixel> dlibImage;
		
		if (currentFrame.channels() == 3) 
		{
			imgFormat = link_dev::Data::Imaging::Format::BGR_U8;
			dlib::assign_image(dlibImage, dlib::cv_image<dlib::bgr_pixel>(currentFrame));
			
		}
		else
		{
			imgFormat = link_dev::Data::Imaging::Format::GRAY_U8;
			cv::Mat tempMat;
			cv::cvtColor(currentFrame, tempMat, cv::COLOR_GRAY2BGR, 3);
			dlib::assign_image(dlibImage, dlib::cv_image<dlib::bgr_pixel>(tempMat));
		}
		
		detFaces = net(dlibImage);

		for (auto&& face : detFaces) 
		{
			if (m_toVisualize) 
			{
				cv::Point upperLeft(face.rect.left(), face.rect.top());
				cv::Point bottomRight(face.rect.right(), face.rect.bottom());
				cv::rectangle(currentFrame, upperLeft, bottomRight, cv::Scalar(0, 255, 0));
			}
			
			auto upperLeft = facePositionData.add_tuples();
			upperLeft->set_item1((uint64_t)face.rect.left());
			upperLeft->set_item2((uint64_t)face.rect.top());
			auto bottomRight = facePositionData.add_tuples();
			bottomRight->set_item1((uint64_t)face.rect.right());
			bottomRight->set_item2((uint64_t)face.rect.bottom());
		}

		*result.mutable_positions() = facePositionData;

		if (_configuration.visualise == "on") {
			msg = link_dev::Data::Imaging::Image2DInterfaces::ToSerializableMessage(rereadOpencvMat, imgFormat, _configuration.visualStreamID);
			publishSerializable(msg);
		}
		else {
			auto msg = link_dev::SerializableMessage<link_dev::Data::Imaging::Image2D_Plus_Points>{result, _configuration.imageStreamIDOut};
			publishSerializable(msg);
		}
	}
	catch (const std::exception& ex)
	{
		std::cout << "Exception occured: " << ex.what() << std::endl;
	}
}

