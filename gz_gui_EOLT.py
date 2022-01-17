from guizero import App, Text, TextBox, Combo, PushButton, Box, Picture
import numpy as np
import csv
from time import sleep
import RPi.GPIO as GPIO
import board
import busio
import adafruit_ads1x15.ads1015 as ADS
from adafruit_ads1x15.analog_in import AnalogIn
from fpdf import FPDF
import matplotlib.pyplot as plt
import pandas as pd
# Create the I2C bus
i2c = busio.I2C(board.SCL, board.SDA)

# Create the ADC object using the I2C bus
ads = ADS.ADS1015(i2c)
# Stepper Motor and Sensor head GPIO addresses
DIR = 27   # Direction GPIO Pin
STEP = 17  # Step GPIO Pin
CW = 1     # Clockwise Rotation Plate towards home
CCW = 0    # Counterclockwise Rotation Plate away from home
ENB = 22   # the enable pin - this pin is inverted
head_select_0 = 23
head_select_1 = 24
head_select_2 = 25
head_select_3 = 12
#Set up the analog to digital pins
ads0 = AnalogIn(ads, ADS.P0)
ads1 = AnalogIn(ads, ADS.P1)
ads2 = AnalogIn(ads, ADS.P2)
ads3 = AnalogIn(ads, ADS.P3)

GPIO.setmode(GPIO.BCM)
GPIO.setup(head_select_0, GPIO.OUT)
GPIO.setup(head_select_1, GPIO.OUT)
GPIO.setup(head_select_2, GPIO.OUT)
GPIO.setup(head_select_3, GPIO.OUT)
GPIO.setup(STEP, GPIO.OUT)
GPIO.setup(DIR, GPIO.OUT)
GPIO.output(DIR, CW)
GPIO.setup(5, GPIO.IN, pull_up_down=GPIO.PUD_UP) # Set pin 5 to be an input pin and set initial value to be pulled low (off)
GPIO.setup(21, GPIO.IN, pull_up_down=GPIO.PUD_UP) # Set pin 21 to be an input pin and set initial value to be pulled low (off)

# Initiallizing the Home Switch on pin 5
pin5 = GPIO.input(5)

# pin21 = GPIO.input(21) # to be used for the power down Raspberry Pi

GPIO.setup(ENB, GPIO.OUT)
HALLS = 6
HEADS = 3
delay = .015
readings_table = []
plotlegend = [] 
users=[] # list to hold users - purhapse will change to Dictionary to make easier to add 
item_num_indx = 0 # used as a global index for which test
noise_readings = 100 # how many readings for the noise check

# Item number column headers
# 0-Part Numbers,
# 1-Count Halls,
# 2-halls/head,
# 3-# heads,
# 4-Selected,
# 5-addressed,
# 6-highMax,
# 7-highMin,
# 8-lowMax,
# 9-lowMin,
# 10-diffMax,
# 11-diffLow
# 12-Harness item number 
# 13-Fixture item number
# 14-Noise Threshold
item_numbers = np.array([[107287,8,2,4,1,0,1000,700,-600,-900,1000,-50,204109,124458,200],
                        [107297,8,2,4,1,0,1000,700,-600,-900,818,560,204109,124458,200],
                        [108144,8,2,4,1,0,1000,700,-600,-900,1000,-50,204109,124458,200],
                        [108150,8,2,4,1,0,1000,700,-600,-900,885,654,204109,124458,200],
                        [112497,6,2,3,1,0,1000,700,-600,-900,1000,-50,301404,124734,200],
                        [121248,12,3,4,1,0,1000,700,-600,-900,728,460,301393,124393,200],
                        [121250,18,6,3,0,1,1000,700,-600,-900,609,423,301400,124394,200],
                        [121334,15,5,3,0,1,1000,700,-600,-900,1000,-50,301401,124742,200],
                        [121335,15,5,3,0,1,1000,700,-600,-900,1000,-50,301401,124740,200],
                        [121791,12,6,2,0,1,1000,700,-600,-900,1000,-50,301400,124394,200]])
                            
print(item_numbers)
# Create the item numbers for the Combobox
items =[]
for column in item_numbers:
    items.append(column[0])
print(items)

def flatten_list(_2d_list):
    flat_list = []
    # Iterate through the outer list
    for element in _2d_list:
        if type(element) is list:
            # If the element is of type list, iterate through the sublist
            for item in element:
                flat_list.append(item)
        else:
            flat_list.append(element)  
    return flat_list

# if file missing got error FileNotFoundError: [Errno 2] No such file or directory: 'users.csv'
with open('Users.csv', newline='') as FR:
    reader = csv.reader(FR, delimiter =',')
    # this brings in a list of list of names as such [["John"], ["Julie"]]
    for row in reader:
        users.append(row)
    # but I only want only a list as such ["John", "Julie"]
    # So this will flatten the list
    print(f"unsorted:{users}")
    users = flatten_list(users)
    # sort the list of names alphabetically with sorted()
    users=sorted(users)
    print(users)




def addressed_read_all_halls():
    hall_readings = []
    
#     print("heads: " + str(HEADS))
#     print("heads: " + str(HALLS))
    for i in range(HEADS):
        for j in range(HALLS):
            addressed_hall_number = i * HALLS + j
            
            if j == 0:
#                 print("Hall_0")
                GPIO.output(head_select_0, GPIO.LOW)
                GPIO.output(head_select_1, GPIO.LOW)
                GPIO.output(head_select_2, GPIO.LOW)
                GPIO.output(head_select_3, GPIO.LOW)
            elif j == 1:
#                 print("Hall_1")
                GPIO.output(head_select_0, GPIO.HIGH)
                GPIO.output(head_select_1, GPIO.LOW)
                GPIO.output(head_select_2, GPIO.LOW)
                GPIO.output(head_select_3, GPIO.LOW)
                
            elif j == 2:
#                 print("Hall_2")
                GPIO.output(head_select_0, GPIO.LOW)
                GPIO.output(head_select_1, GPIO.HIGH)
                GPIO.output(head_select_2, GPIO.LOW)
                GPIO.output(head_select_3, GPIO.LOW)
            elif j == 3:
#                 print("Hall_3")
                GPIO.output(head_select_0, GPIO.HIGH)
                GPIO.output(head_select_1, GPIO.HIGH)
                GPIO.output(head_select_2, GPIO.LOW)
                GPIO.output(head_select_3, GPIO.LOW)
            elif j == 4:
#                 print("Hall_4")
                GPIO.output(head_select_0, GPIO.LOW)
                GPIO.output(head_select_1, GPIO.LOW)
                GPIO.output(head_select_2, GPIO.HIGH)
                GPIO.output(head_select_3, GPIO.LOW)
            else:
#       import csv           print("Hall_5")
                GPIO.output(head_select_0, GPIO.HIGH)
                GPIO.output(head_select_1, GPIO.LOW)
                GPIO.output(head_select_2, GPIO.HIGH)
                GPIO.output(head_select_3, GPIO.LOW)
            
            if i == 0:
                # print("0 - Hall Number: " + str(addressed_hall_number))
                try:
                    hall_readings.append(ads0.value)
                except OSError as e:
                    print("OSError at hall_readings.append(ads0.value)")
                    print(e)
                    # Try again and continue and learn if just trying to read the ADS crashes again.
                    # This is from an OSError with the adafruit library - found a different library
                    # http://abyz.me.uk/lg/examples.html
                    hall_readings.append(ads0.value)
                    
                
            elif i == 1:
                # print("1 - Hall Number: " + str(addressed_hall_number))
                try:
                    hall_readings.append(ads1.value)
                except OSError as e:
                    print("OSError at hall_readings.append(ads1.value)")
                    print(e)
                    # try again 
                    hall_readings.append(ads1.value)
                                   
                
            elif i == 2:
                # print("2 - Hall Number: " + str(addressed_hall_number))
                try:
                    hall_readings.append(ads2.value)
                except OSError as e:
                    print("OSError at hall_readings.append(ads2.value)")
                    print(e)
                    # try again 
                    hall_readings.append(ads2.value)
            
            else:
                # print("3 - Hall Number: " + str(addressed_hall_number))
                try:
                    hall_readings.append(ads3.value)
                except OSError as e:
                    print("OSError at hall_readings.append(ads3.value)")
                    print(e)
                    # try again 
                    hall_readings.append(ads3.value)

     
#     print(hall_readings)
    return(hall_readings)

def double_click():
    print("Double Clicked")
    
#     name = app.question("Hello", "What name do you want to add to the list?")
#     # If cancel is pressed, None is returned
#     # so check a name was entered
#     if name is not None:
#         print(name)
#         users.append(str(name))
#     with open("Users.csv", 'w', newline='') as f:
#         writer = csv.writer(f)
#         writer.writerow([users])

def update_harnes_fixture_lbl():
    global item_num_indx
    print(selected_item.value)
    item_num_indx = items.index(int(selected_item.value))
    harness = str(item_numbers[item_num_indx][6])
    fixture = str(item_numbers[item_num_indx][7])
    use_harness.value = harness
    use_fixture.value = fixture
    save_btn.enabled=True
    tst_btn.enabled = True

    
def save_test():
    print("Save Test")

def begin_test():
    global item_num_indx
    report_txt = "" #This will hold all the report test and post it to the test report textbox see how it works
    UUT = item_numbers[item_num_indx][0]
    report_txt += f"Testing a {UUT} \n"
    HALLS = item_numbers[item_num_indx][2]
    report_txt += f"Using {HALLS} Halls per Head\n"
    HEADS = item_numbers[item_num_indx][3]
    report_txt += f"With {HEADS} Heads\n"
       
    
    # Obtain test configuration
    for h in range(HALLS * HEADS):
        plotlegend.append("hall: "+ str(h))
    
    GPIO.output(ENB, True)
    # Homing
    global pin5
    
    while pin5 == 0:   
        GPIO.output(STEP, GPIO.HIGH)
        sleep(delay) # need pulse for stepper motor controller to move
        GPIO.output(STEP, GPIO.LOW)
        pin5 = GPIO.input(5)

    # rapid move to starting
    GPIO.output(DIR, CCW)
    for s in range(500):
        GPIO.output(STEP, GPIO.HIGH)
        sleep(delay/10)
        GPIO.output(STEP, GPIO.LOW)
    
    # Read sesnors once to prime, or noise reduction, of the ADS1X15 sensor
    addressed_read_all_halls()
    # Noise Check
    for n in range(noise_readings):
        readings_table.append(addressed_read_all_halls())
        
    # testing steps
    for s in range(1100-500):
        GPIO.output(STEP, GPIO.HIGH)
        sleep(delay/10)
        GPIO.output(STEP, GPIO.LOW)
        step = int(round(s/18,0))
        # Write the test readings to the table        
        readings_table.append(addressed_read_all_halls())
        
    df = pd.DataFrame(readings_table)
    # Calculate noise - 1st derivitive
    noise_results = df.iloc[:noise_readings, 0:((HALLS * HEADS))].diff(axis=0, periods = 1).abs().max().to_frame()
    noise_results.columns = ['Noise']
    noise_results["Halls"] = plotlegend
#     Dump noise result to a file?
#     noise_results.to_csv('noise_results.csv', sep=',')
    
    
    report_txt += str(noise_results) + "\n"
    result_txtbox.value = report_txt
    
    # return the plate back home
    GPIO.output(DIR, CW)    
    for r in range(1100):
        GPIO.output(STEP, GPIO.HIGH)
        sleep(delay/10)
        GPIO.output(STEP, GPIO.LOW)

        step = int(60-round(r/18,0))  

    # Turn on the stepper motor controller
    GPIO.output(ENB, False)
    GPIO.cleanup()
    
    # Make the Counts chart
    plt.plot(readings_table)
    plt.rcParams["figure.figsize"] = [7.50, 3.50] 
    plt.xlabel('Steps')
    plt.ylabel('Counts')
    plt.savefig('Counts.png')
    
    # plt.show() # if you would like a window pop up
    # Make Noise Chart
    #https://www.tutorialspoint.com/how-to-create-a-matplotlib-bar-chart-with-a-threshold-line
    plt.rcParams["figure.figsize"] = [7.50, 3.50]
    # plt.rcParams["figure.autolayout"] = True
    threshold = item_numbers[item_num_indx][14] # 14 is the threshold
    a_threshold = np.maximum(noise_results["Noise"] - threshold, 0)
    b_threshold = np.minimum(noise_results["Noise"], threshold)
    x = range(HALLS * HEADS)
    fig, ax = plt.subplots()
    ax.bar(x, b_threshold, 0.35, color="green")
    ax.bar(x, a_threshold, 0.35, color="red", bottom=b_threshold)
    plt.axhline(threshold, color='red', ls='dotted')
    plt.savefig('Noise.png')
    
    # Now resize saved figures for the GUI
    
    # Create PDF report
    pdf = FPDF('P', 'mm', 'letter')
    pdf.add_page()
    pdf.image('EOLT_report.png', x = 0, y = 0, w = 203, h = 279, type = 'PNG')
    pdf.image('Noise.png', x = 0, y = 55, w = 97, h = 82, type = 'PNG')
    pdf.image('Counts.png', x = 100, y = 55, w = 97, h = 82, type = 'PNG')
    pdf.set_font('helvetica', 'B', 16)
    pdf.text(23, 40, '121250')
    pdf.text(23, 51.5, 'January 3, 2022')
    pdf.text(127, 40, 'B12345')
    pdf.text(127, 51.5, '01032022r12m3')
    #set font for results
    pdf.set_font('helvetica', size=10)
    
    
    # Creating an empty list
    rows = []
    # Iterating through the columns of
    # dataframe
    results = noise_results[['Halls', 'Noise']]
    for column in results.columns:
        # Storing the rows of a column
        # into a temporary list
        li = results[column].tolist()
         # appending the temporary list
        rows.append(li)

    print(f"Rows {rows}")
    print("plot legend")
    print(plotlegend)
    print(f"second column in rows {rows[1]}")
    row_pos=200 #counter for which row in loop
    for row in (rows):
        pdf.text(10, row_pos,str(row))
        print(row)
        row_pos += 5

#     pdf.write(5, str(results))
    pdf.output('tuto1.pdf', 'F')
    
    
        
    
    
def plot_noise():
    print("Noise Plot")

def plot_counts():
    print("plot_counts")



app = App(layout="grid", title = "EOLT", width = 1500, height = 650)
button_box=Box(app, layout="grid",  grid=[0, 0, 3, 6], border=3)

# Item Numbers
itm_num_lbl = Text(button_box, text="1. Choose - Item Number:", size=20, grid=[0,1], align="left")
selected_item = Combo(button_box, grid=[1, 1, 2, 1], width=15, options=items, command=update_harnes_fixture_lbl)
selected_item.text_size=20
# Serial Numbers
serial_label = Text(button_box, text="2. Enter - Serial Number:", size=20, grid=[0,2], align="left")
serial_num_txtbox = TextBox(button_box, grid=[1, 2, 2, 1], width=17, command=update_harnes_fixture_lbl)
serial_num_txtbox.text_size = 20
# User 
user_lbl = Text(button_box,text="3. Select - User:", size=20, grid=[0, 3], align="left" )
user_lbl.when_double_clicked = double_click
user_name_cmb = Combo(button_box,options=users, grid=[1,3, 2, 1], align="left", width=15)
user_name_cmb.text_size=20
# Use fixture and harness
harness_lbl = Text(button_box, text="4. Use Harness:", size=20, grid=[0, 4], align="left")
use_harness = TextBox(button_box, enabled=True, width=17, grid=[1, 4, 2, 1])
use_harness.text_size=20
use_harness.bg = "#999999"
fixture_lbl = Text(button_box, text="5. Use Fixture:", size=20, grid=[0,5], align="left")
use_fixture = TextBox(button_box, enabled=True, width=17, grid=[1, 5, 2, 1])
use_fixture.bg = "#999999"
use_fixture.text_size=20
# Buttons
tst_btn = PushButton(button_box, command=begin_test, text = "6. Begin Test", grid=[0,6])
tst_btn.text_size = 30
tst_btn.enabled = False
save_btn = PushButton(button_box, command=save_test, text = "7. Save Test", grid=[1,6])
save_btn.text_size = 30
save_btn.enabled = False
graph_box = Box(app, layout="grid", grid=[6,0,6,6], border=3)
noise_btn = PushButton(graph_box, command=plot_noise, image="Noise_400x300.png", grid=[0,0])
counts_btn = PushButton(graph_box, command=plot_counts, image="Counts_400x300.png", grid=[1,0])

results_box = Box(app, layout='grid', grid=[0,6, 7,6], border=3)
results_lbl = Text(results_box, grid=[0,0], text="Test Results: Passed")
results_lbl.text_size = 30
results_lbl.bg='green'

# make a multiline test from list
fill = ""
for l in item_numbers:
    fill += str(l) +"\n"
    

result_txtbox=TextBox(results_box, grid=[0,1], text=fill, align="left", multiline=True, scrollbar=True)
result_txtbox.height = 15
result_txtbox.width = 175

app.display()

