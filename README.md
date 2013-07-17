IMAR-C
======

IMAR-C (Intermedia Activity Recognition in C language) is a set of activities recognition libraries allowing the robot NAO to comprehend the outside world.

The robot must be capable of recognizing certain human activities. To go farther and to allow the robot continuing its learning he will be able to:
* ask to the person to confirm its activity;
* make a decision with regard to the probability of the activity.

## Activities
For the moment the activities had to be recorded in a 2-second video (our video analysis programm is too long):

	- applaud
	- fall
	- walk
	- sit down
	- stand up
	- drink coffee (ie. drink something)
	- read a book / newspaper (turn over a page)
	- open / close a door
	(- put earphones)
	- write	 
	- shake hands

## Sources:
* SVM: [http://www.csie.ntu.edu.tw/~cjlin/libsvm/](http://www.csie.ntu.edu.tw/~cjlin/libsvm/);
* KMeans: [http://www.cs.umd.edu/~mount/Projects/KMeans/](http://www.cs.umd.edu/~mount/Projects/KMeans/);
* STIPs: [http://lear.inrialpes.fr/people/wang/dense_trajectories](http://lear.inrialpes.fr/people/wang/dense_trajectories);
* NAO sofwares: [https://developer.aldebaran-robotics.com/home/](https://developer.aldebaran-robotics.com/home/).

## Members:
HUILON He and ROUALDES Fabien (Institut Mines-Télécom)
