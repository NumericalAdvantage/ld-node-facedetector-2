/*
 * This file is part of project link.developers/ld-node-facedetector-2.
 * It is copyrighted by the contributors recorded in the version control history of the file,
 * available from its original location https://gitlab.com/link.developers.beta/ld-node-facedetector-2.
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include "Facedetector.hpp"

int main(int argc, char **argv)
{
	std::cout << "This service implements link.developers/RFC:026/ld-node-facedetector v1.0.0.\n";

	try 
	{
		DRAIVE::Link2::NodeResources nodeResources { "l2spec:/link_dev/ld-node-facedetector-2", argc, argv };
        DRAIVE::Link2::NodeDiscovery nodeDiscovery { nodeResources };
        
        DRAIVE::Link2::ConfigurationNode rootNode = nodeResources.getUserConfiguration();
        DRAIVE::Link2::OutputPin outputPin{nodeDiscovery, nodeResources, "video_output"};
        DRAIVE::Link2::InputPin inputPin{nodeDiscovery, nodeResources, "video_input"};

        DRAIVE::Link2::SignalHandler signalHandler {};
        signalHandler.setReceiveSignalTimeout(-1);

		link_dev::Services::FaceDetector fDetector(signalHandler, nodeResources, 
		                                           nodeDiscovery, outputPin, inputPin, 
										           rootNode.getBoolean("DebugMode"),
										           rootNode.getString("PathToModelFile"));
										 
		return fDetector.Run();
	} 
	catch (const std::exception& e) 
	{
		std::cout << e.what() << std::endl;
		return 1;
	}
	return 0;
}
