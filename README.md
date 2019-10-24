# ld-node-facedetector-2

[![build status](https://gitlab.com/link.developers.beta/ld-node-facedetector-2/badges/master/build.svg)](https://gitlab.com/link.developers.beta/ld-node-facedetector-2/commits/master)

## Motivation and Context

- When supplied a video, this node detects human faces in each indivdual frame of the video.
- This node runs in 2 modes depending upon the state of the "DebugMode" configuration parameter.
- In normal mode this node supplies the following offer:
```
      table ImageWithBoundingBox
      {
          imageWithFace : link_dev.Image;
          boxes : [BoundingBox];
      }

      where BoundingBox is basically:

      table BoundingBox
      {
          left : int32;
          top : int32;
          right : int32;
          bottom : int32;
      }
```
- So, in normal mode you get the original image back, with a set of set of 4 points which form rectangles in the image meant to denote faces that were found in the image. 
- In debug mode, you get a video feed in real time (assuming a sufficiently fast computer. From Reference[3], this is a computer with a NVIDIA Titan X GPU) with an overlay in black which shows you where the face(s) predictions were made on the frame.
- Internally this node uses dlib to do all the machine learning stuff. It uses a pretrained model from dlib and makes heavy use of sample programs provided by dlib.

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
4. http://dlib.net/dlib/dnn/loss_abstract.h.html54. https://arxiv.org/pdf/1502.00046.pdf
5. http://dlib.net/face_detection_ex.cpp.html
6. http://dlib.net/dnn_introduction_ex.cpp.html
7. http://dlib.net/dnn_introduction2_ex.cpp.html
8. http://blog.dlib.net/2016/10/easily-create-high-quality-object.html

## Maintainers

- Ritwik Ghosh (maintainer, original author)
- Based upon a link node previously written by Renzhou Ge.
