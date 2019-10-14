/*
 * This file is part of project link.developers/ld-node-facedetector.
 * It is copyrighted by the contributors recorded in the version control history of the file,
 * available from its original location https://gitlab.com/link.developers.beta/ld-node-facedetector.
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include <dlib/image_transforms.h>
#include <dlib/image_processing.h>
#include <dlib/data_io.h>
#include <dlib/opencv/cv_image.h>
#include <link_dev/Interfaces/OpenCvToImage.h>
#include "Facedetector.hpp"
#include "data_generated.h"

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
		std::vector<dlib::mmod_rect> detFaces;
		dlib::matrix<dlib::rgb_pixel> dlibImage;
		bool grayScale = false;
		
		if (currentFrame.channels() == 3) 
		{
			dlib::assign_image(dlibImage, dlib::cv_image<dlib::bgr_pixel>(currentFrame));
			grayScale = true;
		}
		else
		{
			cv::Mat tempMat;
			cv::cvtColor(currentFrame, tempMat, cv::COLOR_GRAY2BGR, 3);
			dlib::assign_image(dlibImage, dlib::cv_image<dlib::bgr_pixel>(tempMat));
		}
	
		// Now tell the face detector to give us a list of bounding boxes
        // around all the faces it can find in the image.
		detFaces = m_neuralNet(dlibImage);

		for (auto&& face : detFaces)
		{
			if (m_debugActivated) 
			{
				//For each face that is found in currentFrame, add an overlay (in black) to the image 
				//such that when the image is viewed the bounding boxes can be seen in the image.
				cv::Point upperLeft(face.rect.left(), face.rect.top());
				cv::Point bottomRight(face.rect.right(), face.rect.bottom());
				cv::rectangle(currentFrame, upperLeft, bottomRight, cv::Scalar(0, 255, 0));
				//Publish the image with bounding boxes overlay for debugging.
				if(grayScale)
				{
					m_outputPin.push(link_dev::Interfaces::ImageFromOpenCV(currentFrame, 
					                                             link_dev::Format::Format_GRAY_U8));
				}
				else
				{
					m_outputPin.push(link_dev::Interfaces::ImageFromOpenCV(currentFrame, 
					                                             link_dev::Format::Format_BGR_U8));	
				}
			}
			else
			{
				//For each face that is found in currentFrame, create a BoundingBox flatbuffer
				//object and push it onto the mesh.
				if(grayScale)
				{
					ImageWithBoundingBoxT img{};
					img.imageWithFace(link_dev::Interfaces::ImageFromOpenCV(currentFrame, 
					                                            link_dev::Format::Format_GRAY_U8));
					

				}
				else
				{
					
				}
				
						

			}
			

			
			auto upperLeft = facePositionData.add_tuples();
			upperLeft->set_item1((uint64_t)face.rect.left());
			upperLeft->set_item2((uint64_t)face.rect.top());
			auto bottomRight = facePositionData.add_tuples();
			bottomRight->set_item1((uint64_t)face.rect.right());
			bottomRight->set_item2((uint64_t)face.rect.bottom());
		}

		*result.mutable_positions() = facePositionData;

		if (m_toVisualize) 
		{
			msg = 
			link_dev::Data::Imaging::Image2DInterfaces::ToSerializableMessage(currentFrame, 
			                                                         imgFormat, 
																	 _configuration.visualStreamID);
			publishSerializable(msg);
		}
		else 
		{
			auto msg = 
			link_dev::SerializableMessage<link_dev::Data::Imaging::Image2D_Plus_Points>{result, 
			                                                     _configuration.imageStreamIDOut};
			publishSerializable(msg);
		}
	}
	catch (const std::exception& ex)
	{
		std::cout << "Exception occured: " << ex.what() << std::endl;
	}
}

