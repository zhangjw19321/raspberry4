############################  Serial ########################## 
######### final version 1 ##############
import serial
from time import sleep




class Motor:
	# sample: "MICEN" M means motor
	def __init__(self,p_port):               	
		self.motor_port = p_port.upper()
		if self.motor_port not in ["A","B","C","D"]:
			print("Motor Port Error")
        self.motor_serial = serial.Serial(
		    port='/dev/ttyS0',
		    baudrate = 9600,
		    parity=serial.PARITY_NONE,
		    stopbits=serial.STOPBITS_ONE,
		    bytesize=serial.EIGHTBITS,
		    timeout=0.05
			   )
		init_info = "MI" + str(self.motor_port) + "EN"
		self.motor_serial.write(init_info.encode())
	def speed_check(self,p_speed):
		if abs(p_speed) > 100:
			print("wrong speed parameter, run at max speed value")
			if p_speed > 0:
				return 100
			else:
				return -100
		else:
			return p_speed
	def time_check(self,p_time):
		if p_time <= 0:
			print("wrong time parameter, no moving")
			p_time = 0.01
			return p_time
		else:
			return p_time
	def angle_check(self,p_angle):
		return p_angle
	# sample:"MGCEN" M means motor G means get information
	def get_angle(self):
		get_angle_info = "MG" + str(port) + "EN"
		self.motor_serial.write(get_angle_info.encode())
		readData = self.motor_serial.readline()
		angle = (readData.decode()).strip()
		return angle
	# sample: "MPZC20T10EN" Z means time control and P means power method
	def run_time(self,p_speed,p_time):
		r_speed = speed_check(p_speed)
		p_time = p_time * 1000
		r_time = time_check(p_time)
		run_time_motor_info = "MPZ" + self.motor_port + str(r_speed) + "T" + str(r_time) + "EN"
		self.motor_serial.write(run_time_motor_info.encode())
		readData = self.motor_serial.readline()
	# sample "MPYC20G10EN" Y means angle control and P means power method
	def run_angle(self,p_speed,p_angle):
		r_speed = speed_check(p_speed)
		r_angle = angle_check(p_angle)
		run_angle_motor_info = "MPY" + self.motor_port + str(r_speed) + "G" + str(r_angle) + "EN"
		self.motor_serial.write(run_angle_motor_info.encode())
		readData = self.motor_serial.readline()
	# sample "MPXC20EN" X means real angle speed control and P means power method
	def run_speed(self,p_speed_pwm):
		r_speed = speed_check(p_speed_pwm)
		run_speed_motor_info = "MPX" + self.motor_port + str(r_speed) + "EN"
		self.motor_serial.write(run_speed_motor_info.encode())
		readData = self.motor_serial.readline()


class ColorSensor:
	# sample: "CI1EN" C means color mode 
	def __init__(self,p_port):               	
		self.color_sensor_port = str(p_port)
		if self.color_sensor_port not in ["1","2","3","4"]:
			print("Color Sensor Port Error")
        self.color_sensor_serial = serial.Serial(
		    port='/dev/ttyS0',
		    baudrate = 9600,
		    parity=serial.PARITY_NONE,
		    stopbits=serial.STOPBITS_ONE,
		    bytesize=serial.EIGHTBITS,
		    timeout=0.05
			   )
		init_info = "CI" + str(self.color_sensor_port) + "EN"
		self.color_sensor_serial.write(init_info.encode())
	# sample:"CG1EN" 
	def get_color_information(self):
		get_color_info = "CG" + str(color_sensor_port) + "EN"
		self.color_sensor_serial.write(get_color_info.encode())
		readData = self.color_sensor_serial.readline()
		angle = (readData.decode()).strip()
		try:
	        color_value = color_str.split("S")[1]
	    except:
	        color_value = "R55G55X55Y5"
	    first = color_value.split("R")[1].split("G")[0]
	    second = color_value.split("G")[1].split("X")[0]
	    third = color_value.split("X")[1].split("Y")[0]
	    fourth = color_value.split("Y")[1]
	    red = int(first,16)
	    green = int(second,16)
	    blue = int(third,16)
	    intensity = int(fourth)
	    return red,green,blue,intensity

class UltrasonicSensor:
		# sample: "UI1EN" C means color mode 
	def __init__(self,p_port):               	
		self.ultrasonic_sensor_port = str(p_port)
		if self.ultrasonic_sensor_port not in ["1","2","3","4"]:
			print("Ultrasonic Sensor Port Error")
        self.ultrasonic_sensor_serial = serial.Serial(
		    port='/dev/ttyS0',
		    baudrate = 9600,
		    parity=serial.PARITY_NONE,
		    stopbits=serial.STOPBITS_ONE,
		    bytesize=serial.EIGHTBITS,
		    timeout=0.05
			   )
		init_info = "UI" + str(self.ultrasonic_port) + "EN"
		self.ultrasonic_sensor_serial.write(init_info.encode())
	# sample:"UG1EN" 
	def get_distance_information(self):
		get_distance_info = "UG" + str(ultrasonic_sensor_port) + "EN"
		self.ultrasonic_sensor_serial.write(get_distance_info.encode())
		readData = self.ultrasonic_sensor_serial.readline()
		distance_info = (readData.decode()).strip()
		try:
	        dis = distance_str.split("S")[1]
	    except:
	        self.d_count += 1
	        dis = "999.99"
	    return dis



# compateble for old use
ser = serial.Serial(
		    port='/dev/ttyS0',
		    baudrate = 9600,
		    parity=serial.PARITY_NONE,
		    stopbits=serial.STOPBITS_ONE,
		    bytesize=serial.EIGHTBITS,
		    timeout=0.05
			   )
def getColor():
    send_data = "Y1EN"
    ser.write(send_data.encode())
    readData = self.ser.readline()
    color_str = (readData.decode()).strip()
    # print(color_str)
    try:
        color_value = color_str.split("S")[1]
    except:
        color_value = "R55G55X55Y5"
    first = color_value.split("R")[1].split("G")[0]
    second = color_value.split("G")[1].split("X")[0]
    third = color_value.split("X")[1].split("Y")[0]
    fourth = color_value.split("Y")[1]
    red = int(first,16)
    green = int(second,16)
    blue = int(third,16)
    intensity = int(fourth)
    # print(red, green,blue,intensity)
    if intensity < 7 or red == green:
        return "Black"
    elif red > green:
        return "Red"
    else:
         return "Green"

def getDistance():
    send_data = "U1EN"
    self.ser.write(send_data.encode())
    readData = self.ser.readline()
    readData1 = self.ser.readline()
    distance_str  = (readData.decode()).strip()
    print("distance str is ", distance_str)
    try:
        dis = distance_str.split("S")[1]
    except:
        self.d_count += 1
        dis = "999.99"
    return dis

if __name__ == "__main__":
	# motor test
    motor = Motor("C")
	motorc_angle = motor.get_angle()
	print("motor c angle is: ", motorc_angle)
	motor.run_time(30,3)
	print("motor c run 3s at 30")
	sleep(3)
	motor.run_angle(40,1000)
	print("motor c run 1000 degree at speed 40")	
	sleep(3)
	motor.run_speed(20)
	print("motor c run v at speed 20 for 2s")
	sleep(2)
	motor.run(0)
	# color sensor test
	color_sensor = 
	
    LS.go(70,70,1)
    
