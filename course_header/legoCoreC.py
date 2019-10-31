############################  Serial ########################## 
######### final version 1 ##############
import serial
import time
import threading

class LEGO_Serial():
	ser = []
	A_encoder_num = 0
	B_encoder_num = 0
	C_encoder_num = 0
	D_encoder_num = 0
	threadLock = threading.Lock()
	error_count = 0
	d_count = 0
	def __init__(self):
		# serial port parameter initialtzation
		self.ser = serial.Serial(
		    port='/dev/ttyS0',
		    baudrate = 9600,
		    parity=serial.PARITY_NONE,
		    stopbits=serial.STOPBITS_ONE,
		    bytesize=serial.EIGHTBITS,
		    timeout=0.05
			   )
	def speed_check(self,speed):
		if abs(speed) > 100:
			print("wrong speed parameter")
			return "0"
		else:
			return speed
	def time_check(self,t):
		if t <= 0:
			print("wrong time parameter")
			t = 0.01
			return t
		else:
			return t
	# A10B10C10D10T1EN
	def send_data(self,a_motor = "A0", b_motor="B0", c_motor="C0", d_motor="D0", r_time = 10):
		motor_parameter = str(a_motor) + str(b_motor) + str(c_motor) + str(d_motor) + "T" + str(r_time)
		send_data = motor_parameter + "E"+"N"
		self.ser.write(send_data.encode())
		# print("come here")
		# print("send data is: ", send_data)
	def getColor(self):
	    send_data = "Y1EN"
	    self.ser.write(send_data.encode())
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
	def getRawColor(self):
	    send_data = "Y1EN"
	    self.ser.write(send_data.encode())
	    readData = self.ser.readline()
	    readData1 = self.ser.readline()
	    color_str = (readData.decode()).strip()
	    try:
	        color_value = color_str.split("S")[1]
	    except:
	        color_value = "555555"
	    if len(color_value) == 6:
	        red = int(color_value[:2], 16)
	        green = int(color_value[2:4],16)
	        blue = int(color_value[4:],16)
	        return red, green,blue
	    else:
	        return 85.0,85.0,85.0
	def getIntensity(self):
	    send_data = "Y1EN"
	    self.ser.write(send_data.encode())
	    readData = self.ser.readline()
	    readData1 = self.ser.readline()
	    color_str = (readData.decode()).strip()
	    try:
	        color_value = color_str.split("S")[1]
	    except:
	        color_value = "555555"
	    if len(color_value) == 6:
	        red = int(color_value[:2], 16)
	        green = int(color_value[2:4],16)
	        black = int(color_value[4:],16)
	        intensity = (red + green + black)/3
	        return intensity
	    else:
	        return 55.0
	def getDistance(self):
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
	def A_motor_timed(self,a_motor_speed,r_time):
	    A_motor_speed = self.speed_check(a_motor_speed)
	    A_motor_time = self.time_check(r_time) * 1000
	    A_motor_parameter = "A" + str(A_motor_speed)
	    self.send_data(a_motor = A_motor_parameter, r_time = A_motor_time)
	    readData = self.ser.readline()
	    readData1 = self.ser.readline()
	# sample B10T10
	def B_motor_timed(self,b_motor_speed,r_time):
	    B_motor_speed = self.speed_check(b_motor_speed)
	    B_motor_time = self.time_check(r_time) * 1000
	    B_motor_parameter = "B" + str(B_motor_speed)
	    self.send_data(b_motor=B_motor_parameter,r_time = B_motor_time)
	    readData = self.ser.readline()
	    readData1 = self.ser.readline()    
	def C_motor_timed(self,c_motor_speed,r_time):
	    C_motor_speed = self.speed_check(c_motor_speed)
	    C_motor_time = self.time_check(r_time) * 1000
	    C_motor_parameter = "C" + str(c_motor_speed)
	    self.send_data(c_motor = C_motor_parameter, r_time = C_motor_time)
	    readData = self.ser.readline()
	    readData1 = self.ser.readline()
	def D_motor_timed(self,d_motor_speed,r_time):
	    D_motor_speed = self.speed_check(d_motor_speed)
	    D_motor_time = self.time_check(r_time) * 1000
	    D_motor_parameter = "D" + str(d_motor_speed)
	    self.send_data(d_motor = D_motor_parameter,r_time = D_motor_time)
	    readData = self.ser.readline()
	    readData1 = self.ser.readline()
	# samle A10T0,move forever, set T to 0
	# sample A10T0
	def A_motor_run(self,a_motor_speed,run_time=0):
	    A_motor_speed = self.speed_check(a_motor_speed)
	    A_motor_parameter = "A" + str(A_motor_speed)
	    self.send_data(a_motor = A_motor_parameter,r_time = 0)
	# sample B10T0
	def B_motor_run(self,b_motor_speed,r_time=0):
	    B_motor_speed = self.speed_check(b_motor_speed)
	    B_motor_parameter = "B" + str(B_motor_speed)
	    self.send_data(b_motor = B_motor_parameter,r_time = 0)
	def C_motor_run(self,c_motor_speed,r_time=0):
	    C_motor_speed = self.speed_check(c_motor_speed)
	    C_motor_parameter = "C" + str(C_motor_speed)
	    self.send_data(c_motor = C_motor_parameter,r_time = 0)
	def D_motor_run(self,d_motor_speed,r_time=0):
	    D_motor_speed = self.speed_check(d_motor_speed)
	    D_motor_parameter = "D" + str(D_motor_speed)
	    self.send_data(d_motor = D_motor_parameter,r_time = 0)
	#basic motion function,default motor C--left and D-right
	def go(self,left,right,r_time):
		C_motor_speed = self.speed_check(-left)
		C_motor_parameter = "C" + str(C_motor_speed)
		D_motor_speed = self.speed_check(-right)
		D_motor_parameter = "D" + str(D_motor_speed)
		r_time = self.time_check(r_time) * 1000
		self.send_data(c_motor = C_motor_parameter, d_motor = D_motor_parameter,r_time = r_time)
		readData = self.ser.readline()
		readData1 = self.ser.readline()
		time.sleep(r_time/1000.0)
	def run(self,left,right,r_time):
	    # modify the head
		C_motor_speed = self.speed_check(-left)
		C_motor_parameter = "C" + str(C_motor_speed)
		D_motor_speed = self.speed_check(-right)
		D_motor_parameter = "D" + str(D_motor_speed)
		r_time = self.time_check(r_time) * 1000
		self.send_data(c_motor = C_motor_parameter, d_motor = D_motor_parameter,r_time = r_time)
		readData = self.ser.readline()
		readData1 = self.ser.readline()
		# print("run code angle is",readData)
	def move(self,speed):
		C_motor_speed = self.speed_check(-speed)
		C_motor_parameter = "C" + str(C_motor_speed)
		D_motor_speed = self.speed_check(-speed)
		D_motor_parameter = "D" + str(D_motor_speed)
		r_time = 0
		self.send_data(c_motor = C_motor_parameter, d_motor = D_motor_parameter,r_time = r_time)
		readData = self.ser.readline()
		readData1 = self.ser.readline()
		# print("move data is",readData)
	def shake_head(self,speed,r_time):
	    A_motor_speed = self.speed_check(speed)
	    A_motor_parameter = "A" + str(A_motor_speed)
	    r_time = self.time_check(r_time) * 1000
	    self.send_data(a_motor = A_motor_parameter, r_time = r_time)
	    readData = self.ser.readline()
	    #readData1 = self.ser.readline()
	    # print("shake head data is",readData)
	    
	def shake_and_move(self, speedA, speedC, speedD, r_time):
	    A_motor_speed = self.speed_check(speedA)
	    A_motor_parameter = "A" + str(A_motor_speed)
	    C_motor_speed = self.speed_check(speedC)
	    C_motor_parameter = "C" + str(C_motor_speed)
	    D_motor_speed = self.speed_check(speedD)
	    D_motor_parameter = "D" + str(D_motor_speed)
	    r_time = self.time_check(r_time) * 1000
	    self.send_data(a_motor=A_motor_parameter, c_motor = C_motor_parameter, d_motor = D_motor_parameter,r_time = r_time)
	    readData = self.ser.readline()

	# default motor is C and D
	def stop(self):
		r_time = 1
		self.C_motor_timed(0,r_time)
		self.D_motor_timed(0,r_time)
		# print("stop data is",readData)
	# stop all motors
	def stop_all(self):
		self.A_motor_timed(0,r_time = 1)
		self.B_motor_timed(0,r_time = 1)
		self.C_motor_timed(0,r_time = 1)
		self.D_motor_timed(0,r_time = 1)
		readData = self.ser.readline()
		readData1 = self.ser.readline()
		# print("stop all data is",readData)
	# sample as code infomation, now not used
	def go_encoder(self,left_pwm,right_pwm,r_time):
		self.threadLock.acquire()
		self.C_motor_timed(self,-right_pwm,r_time)
		self.D_motor_timed(self,-left_pwm,r_time)
		readData = self.ser.readline()
		self.threadLock.release()
		readData = str(readData.decode())
		L_index = readData.find('L')
		R_index = readData.find('R')
		E_index = readData.find('E')
		if R_index != -1 and E_index != -1 and R_index-L_index>1 and E_index-R_index>1:
			Left_num = int(readData[L_index+1:R_index])
			Right_num = int(readData[R_index+1:E_index])
		else:
			self.error_count +=1
			Left_num = -self.Left_encoder_num
			Right_num = -self.Right_encoder_num
		# print('error_count:'+str(self.error_count))
		self.Left_encoder_num = -Left_num
		self.Right_encoder_num = -Right_num
		return (self.Left_encoder_num),(self.Right_encoder_num)
if __name__ == "__main__":
    LS = LEGO_Serial()
    from time import sleep
    LS.go(70,70,1)
    
