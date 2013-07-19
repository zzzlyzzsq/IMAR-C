from naoqi import ALProxy

# Replace "127.0.0.1" with the IP of your NAO
leds = ALProxy("ALLeds","127.0.0.1",9559)
# Print the names of all the groups
print(leds.listGroups())
