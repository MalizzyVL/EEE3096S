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

	if pin == 18:
		count_up
	else if pin == 16:
		count_down

	#display binary digit

	return

def count_up():
	count += 1

	if count == 9:
		count = 0
	#convert decimal to binary
	bin_count = bin(count)
	

def count_up():
	count -= 1

	if count < 0:
		count = 8
		
	bin_count = bin(count)



GPIO.add_event_detect(16, GPIO.RISING, callback=my_callback,bouncetime=300)
GPIO.add_event_detect(18, GPIO.RISING, callback=my_callback,bouncetime=300)
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
