# import Relevant Librares
import RPi.GPIO as GPIO     # importing GPIO library
import time                 # importing time library for delay


GPIO.setwarnings(False)
GPIO.setmode(GPIO.BOARD)     # enable BOARD pin numberings
GPIO.setup(11,GPIO.OUT)      # Set pin 11 as output
GPIO.setup(13,GPIO.OUT)       # Set pin 13 as output
GPIO.setup(15,GPIO.OUT)      # Set pin 15 as output
GPIO.setup(16, GPIO.IN, pull_up_down=GPIO.PUD_DOWN) #set button 1 as input
GPIO.setup(18, GPIO.IN, pull_up_down=GPIO.PUD_DOWN) #set button 2 as input

count = 0

def main():
    # making the button intrupts and link to handle method
    
def my_callback(pin):

	if pin == 18:		#button 2 counts up
		count_up
	else if pin == 16:		#button 1 counts down
		count_down	


	outputs = [(0,0,0), (0,0,1),(0,1,0),(0,1,1),(1,0,0),(1,0,1),(1,1,0),(1,1,1)]
	GPIO.output(11, outputs[count][0])  #set state of led1
    GPIO.output(13, outputs[count][1])  #set state of led2
    GPIO.output(15, outputs[count][2])  #set state of led3
	
	return

def count_up():
	count += 1

	if count == 8:
		count = 0			#counter goes back to zero after 8
	

def count_up():
	count -= 1

	if count < 0:
		count = 8			#counter wraps around and goes to 8 after 0


GPIO.add_event_detect(16, GPIO.RISING, callback=my_callback,bouncetime=300)  #interrupt for button 1
GPIO.add_event_detect(18, GPIO.RISING, callback=my_callback,bouncetime=300)  #interrupt for button 2
# Only run the functions if 
if __name__ == "__main__":
    # Make sure the GPIO is stopped correctly
    try:
        while True:
            main()
    except KeyboardInterrupt:
        print("Exiting gracefully")
        # Turn off your GPIOs here
        GPIO.cleanup()
    except:
        print("Some other error occurred")
