# ld-node-facedetector-2

[![build status](https://gitlab.com/link.developers.beta/ld-node-facedetector-2/badges/master/build.svg)](https://gitlab.com/link.developers.beta/ld-node-facedetector-2/commits/master)

## Motivation and Context

- When supplied a video, this node detects human faces in each indivdual frame of the video.
- This node runs in 2 modes depending upon the state of the "DebugMode" configuration parameter.
- In normal mode this node supplies the following offer which contains the image in which faces are detected and the bounding box corresponding to each face and the number of faces detected.
```
      table ImageWithBoundingBoxes
      {
          imageWithFace : link_dev.Image;  //Image in which the bounding boxes have been defined.
          boxes : [BoundingBox];           //An array of bounding boxes. 
          numBoundingBoxes : int;          //Number of bounding boxes.
      }

      where BoundingBox is basically:

      table BoundingBox
      {
          x_coordinate : int32; //x co-ordinate of top left corner of the bounding box.
          y_coordinate : int32; //y co-ordinate of top left corner of the bounding box.
          width : int32;        //width of the bounding box.
          height : int32;       //height of the bounding box.
      }
```
- From the RFC which this node implements: "A bounding box MUST be described by the (u,v) coordinate of the top left corner and its width and height in pixels.".
- Note: A "bounding box" describes a subset of the image for which it is defined.
- So, in normal mode you get the original image back, with a set of set of 4 points which form rectangles in the image meant to denote faces that were found in the image. 
- In debug mode, you get a video feed in real time (assuming a sufficiently fast computer. This is a computer with a NVIDIA Titan X GPU[3]) with an overlay in black which shows you where the face(s) predictions were made on the frame.
- Internally this node uses dlib [1] to implement the algorithm presented in [5]. It uses a pretrained model [3] from dlib and makes heavy use of sample programs provided by dlib [3], [6], [7].

## Configuration

- DebugMode - boolean - Enables/disables debug mode as described above. Default value is false.
- PathToModelFile - string - Used to indicate where the model for running the neural network is saved on disk. 

## The node in action

- When you run the node in debug mode with ld-node-imageviewer-2 and ld-node-webcam-2, you will see an overlay added to the video around the places in the video where a face was detected. 

## Installation

```
conda install ld-node-facedetector-2
```

## Usage example

```
ld-node-facedetector-2 --instance-file instance.json
```

## Specification

This node implements the following RFCs:
- https://draive.com/link_dev/rfc/026/README/index.html

## Contribution

Your help is very much appreciated. For more information, please see our [contribution guide](./CONTRIBUTING.md) and the [Collective Code Construction Contract](https://gitlab.com/link.developers/RFC/blob/master/001/README.md) (C4).

## References
1. http://dlib.net/ml.html
2. http://dlib.net/dlib/dnn/layers_abstract.h.html
3. http://dlib.net/dnn_mmod_face_detection_ex.cpp.html
4. http://dlib.net/dlib/dnn/loss_abstract.h.html
5. https://arxiv.org/pdf/1502.00046.pdf
6. http://dlib.net/face_detection_ex.cpp.html
7. http://dlib.net/dnn_introduction2_ex.cpp.html

## Maintainers

- Ritwik Ghosh (maintainer, original author)
- Based upon a link node previously written by Renzhou Ge.
