import sys
from naoqi import ALProxy

try:	
	# Replace "127.0.0.1" with the IP of your NAO
	postureProxy = ALProxy("ALRobotPosture","127.0.0.1",9559)
except Exception, e:
	print "Could not create proxy to ALRobotPosture"
	print "Error was: ", e

if (len(sys.argv) != 2):
	print "Usage: ./goToPosture.py <robot posture>"
	print "Postures available: "
	
	# Print the names of all the postures
	postures = postureProxy.getPostureList()
	for i in range(0,len(postures) - 1):
		print(postures[i])
else:
	postureProxy.goToPosture(sys.argv[1],0.5);

