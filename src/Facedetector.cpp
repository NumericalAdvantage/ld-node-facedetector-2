/*
 * This file is part of project link.developers/ld-node-facedetector-2.
 * It is copyrighted by the contributors recorded in the version control history of the file,
 * available from its original location https://gitlab.com/link.developers.beta/ld-node-facedetector-2.
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

#define INITIAL_SIZE 1024

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

void link_dev::Services::FaceDetector::DetectFaces(const cv::Mat& incomingFrame) 
{
	cv::Mat currentFrame(incomingFrame);

	try
	{
		dlib::matrix<dlib::rgb_pixel> dlibImage;
		bool grayScale = false;
		ImageWithBoundingBoxT imageAndBB; /*this is a flatbuffer struct which is used to hold the 
		                                    data which will be serialized by the output pin 
											push() call*/
		
		/*Create a dlib image from the currentFrame because we want to pass the 
		  currentFrame to a dlib neural network to get the (face) bounding box predictions. */
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
	
		/*Now tell the face detector to give us a list of bounding boxes
          around all the faces it can find in the image.*/
		std::vector<dlib::mmod_rect>  detFaces = m_neuralNet(dlibImage);

		/*Iterate over the set of bounding boxes for faces found in the currentFrame by the 
		  neural network.*/
		for (auto&& face : detFaces)
		{
			if (m_debugActivated) 
			{
				/*For each face that is found in currentFrame, add an overlay (in black) to the 
				  image.*/
				cv::Point upperLeft(face.rect.left(), face.rect.top());
				cv::Point bottomRight(face.rect.right(), face.rect.bottom());
				cv::rectangle(currentFrame, upperLeft, bottomRight, cv::Scalar(0, 255, 0));
			}
			else
			{
				/*For each face that is found in currentFrame, create a BoundingBoxT object and 
				  add a pointer to this object to the vector of bounding boxes in the flatbuffer.*/
				BoundingBoxT currentBB;
				currentBB.left = face.rect.left();
				currentBB.top = face.rect.top();
				currentBB.right = face.rect.right();
				currentBB.bottom = face.rect.bottom();
				
				imageAndBB.boxes.push_back(std::make_unique<BoundingBoxT>(currentBB));
			}
		}

		if (m_debugActivated)
		{
			/*Publish the image with bounding boxes overlay for debugging onto the mesh.
			  Note how we do not publish any flatbuffer object in this mode.*/
			if(grayScale)
			{
				m_outputPin.push(link_dev::Interfaces::ImageFromOpenCV(currentFrame, 
			                                           link_dev::Format::Format_GRAY_U8),
													   "l2offer:/debug_video_output");	
			}
			else
			{
				m_outputPin.push(link_dev::Interfaces::ImageFromOpenCV(currentFrame, 
				                                       link_dev::Format::Format_BGR_U8),
													   "l2offer:/debug_video_output");	
			}
		}
		else 
		{
			/*Publish a flatbuffer containing the link_dev::Image and array of bounding boxes
			  onto the mesh.*/
			if(grayScale)
			{
				imageAndBB.imageWithFace = std::make_unique<link_dev::ImageT>
				                           (link_dev::Interfaces::ImageFromOpenCV(currentFrame, 
			                                                 link_dev::Format::Format_GRAY_U8));
				
				m_outputPin.push(imageAndBB, "l2offer:/image_with_bounding_boxes");
			}
			else
			{
				imageAndBB.imageWithFace = std::make_unique<link_dev::ImageT>
				                           (link_dev::Interfaces::ImageFromOpenCV(currentFrame, 
			                                                  link_dev::Format::Format_BGR_U8));

				m_outputPin.push(imageAndBB, "l2offer:/image_with_bounding_boxes");
			}
		}
	}
	catch (const std::exception& ex)
	{
		std::cout << "Exception occured: " << ex.what() << std::endl;
	}
}

