from time import sleep
import sys
import os
import serial
# from memory_module import read_all_memory, read_memory_addr, read_silicon_id, read_manufacturer_id


#####################
# Display FUNCTIONS #
#####################
def display_title_bar():
    # Clears the terminal screen, and displays a title bar.
    os.system('clear')
              
    print("\t**********************************************")
    print("\t***   KIRNO - Serial Hex Byte Comms -      ***")
    print("\t**********************************************")
    print("\n")


def get_user_choice():
    # Let users know what they can do.
    print("\t[1] Show buffer data.")
    print("\t[2] Read memory address.")
    # print("\t[3] Get Silicon ID.")
    # print("\t[4] Get Manufacturer ID.")
    # print("\t[5] Get protected sectors.")
    # print("\t[6] Blank check.")
    # print("\t[7] Get Memory CRC.")
    # print("\t[8] Write all memory.")    
    # print("\t[9] Write memory address.")
    # print("\t[10] Memory Reset.")
    # print("\t[11] Erase all memory.")
    # print("\t[12] CRC from mem.bin.")
    print("\t[q] Quit.")
    
    return input("\tWhat would you like to do? ")


def display_os_info():
    print('Python %s on %s' % (sys.version, sys.platform))
    scriptpath = os.path.realpath(__file__)
    print("[Script path] " + scriptpath)
    sleep(1)


##################
# Menu Functions #
##################
def read_memory_addr (serial_instance):
    s = serial_instance
    addr_str = input("Address to read in hex: ")
    bytes_str = input("How many bytes in dec: ")    

    header_str = '5aa5'
    bint = int(bytes_str)
    # bhex_str = f'{bint:04x}'
    bhex_str = '0003'    
    print(f'read mem: 0x{addr_str} bytes: {bytes_str} bhex: 0x{bhex_str}')

    # bint = int(bytes_str, 16)
    # len_str = f'{bint}'
    len_str = '07'    
    cmd_str = '03'
    crc_str = 'ea10'
    hex_string = header_str + len_str + cmd_str + addr_str + bhex_str + crc_str
    print('hex str: ' + hex_string)
    ba = bytearray.fromhex(hex_string)
      
    s.write(ba)

    loop = True

    while loop:
        answer = s.readline().decode()        
        print(answer)
        loop = False
        
            
def read_all_memory (serial_instance):
    s = serial_instance
    s.write("read all\n".encode('utf-8'))

    loop = True

    print ("Reading all Memory - will take a while")
    while loop:
        answer = s.readline().decode()        

        if 'OK' in answer or 'NOK' in answer:
            loop = False
        elif 'go to binary' in answer:
            print("saving data on mem.bin")
            needed, saved, crc = save_binary_file(s, "mem.bin")
            print(f'Bytes: {needed} Saved: {saved} CRC: {crc}')
            # falta leer el OK final
            loop = False
        else:
            print(answer)

    print(os.popen("./crc16_ccitt mem.bin").read())


def write_all_memory (serial_instance):
    s = serial_instance
    s.write("write all\n".encode('utf-8'))

    loop = True

    print ("Writing all Memory - will take a while")
    while loop:
        answer = s.readline().decode()        

        if 'OK' in answer or 'NOK' in answer:
            loop = False
        elif 'go to binary' in answer:
            print("saving data from mem.bin")
            rx, saved, crc = send_binary_file(s, "mem.bin")
            print(f'Bytes: {rx} Saved: {saved} CRC: {crc}')
            # falta leer el OK final
            loop = False
        else:
            print(answer)

    print(os.popen("./crc16_ccitt mem.bin").read())
    

def crc_from_file ():
    print(os.popen("./crc16_ccitt mem.bin").read())
            

def read_silicon_id (serial_instance):
    s = serial_instance

    flush_serial_buffer(s)
    
    s.write("read sid\n".encode('utf-8'))
    loop = True

    while loop:
        answer = s.readline().decode()        

        if 'OK' in answer or 'NOK' in answer:
            loop = False
        else:
            print(answer)
            

def read_manufacturer_id (serial_instance):
    s = serial_instance
    s.write("read mid\n".encode('utf-8'))

    loop = True

    while loop:
        answer = s.readline().decode()        

        if 'OK' in answer or 'NOK' in answer:
            loop = False
        else:
            print(answer)


def read_protected (serial_instance):
    s = serial_instance
    s.write("read prot\n".encode('utf-8'))

    loop = True

    while loop:
        answer = s.readline().decode()        

        if 'OK' in answer or 'NOK' in answer:
            loop = False
        else:
            print(answer)


def blank_check (serial_instance):
    print ("Blank Checking - will take a few seconds")
    s = serial_instance
    s.write("blank check\n".encode('utf-8'))

    loop = True

    while loop:
        answer = s.readline().decode()        

        if 'OK' in answer or 'NOK' in answer:
            loop = False
        else:
            print(answer)


def erase_all_memory (serial_instance):
    print ("Erasing All Memory - will take a few seconds")
    s = serial_instance
    s.write("erase all\n".encode('utf-8'))

    loop = True

    while loop:
        answer = s.readline().decode()        

        if 'OK' in answer or 'NOK' in answer:
            loop = False
        else:
            print(answer)
            

def write_memory_addr (serial_instance):
    s = serial_instance

    flush_serial_buffer(s)
    addr = input("Address to write in decimal: ")
    value = input("Byte value: ")    

    to_send = f'write addr {addr} {value}\n'
    s.write(to_send.encode('utf-8'))

    loop = True

    while loop:
        answer = s.readline().decode()        

        if 'OK' in answer:
            loop = False
            print("Done!")
        elif 'NOK' in answer:
            loop = False
            print("Error")
        else:
            print(answer)


def get_memory_crc (serial_instance):
    print ("Reading CRC - will take a few seconds")
    s = serial_instance
    s.write("read crc\n".encode('utf-8'))

    loop = True

    while loop:
        answer = s.readline().decode()        

        if 'OK' in answer or 'NOK' in answer:
            loop = False
        else:
            print(answer)

            
#####################
# Utility Functions #
#####################
def check_empty_mem (serial_instance):
    s = serial_instance

    flush_serial_buffer(s)
    checked = 0
    while checked < 524288:
        qtty = s.in_waiting
        if qtty:
            read = s.read(qtty)
            for r in read:
                if r != 0xff:
                    print ("Error in memory addr: " + hex(checked) + " value: " + hex(r))
                    checked = 524288
                    break
                

            checked += qtty

def memory_reset (serial_instance):
    s = serial_instance

    flush_serial_buffer(s)
    
    s.write("mem reset\n".encode('utf-8'))
    loop = True

    while loop:
        answer = s.readline().decode()        

        if 'OK' in answer:
            loop = False
            print("Done!")
        elif 'NOK' in answer:
            loop = False
            print("Error")
        else:
            print(answer)

            
def save_binary_file (serial_instance, file_name):
    s = serial_instance
    needed_bytes = 0
    saved_bytes = 0
    show_bytes = 0
    crc = ""
    
    with open(file_name, "wb") as f:
        loop = True

        while loop:
            answer = s.readline().decode()        
            
            if 'OK' in answer or 'NOK' in answer:
                loop = False
            elif 'CRC' in answer:
                loop = False
                s_crc, crc = answer.split(' ')
            elif 'next' in answer:
                s_next, next_bytes = answer.split(' ')
                next_bytes = int(next_bytes)
                needed_bytes += next_bytes
                s.write("next\n".encode('utf-8'))
                qtty_in_this_chunk = 0

                if show_bytes >= 65536:
                    print(needed_bytes)
                    show_bytes = 0
                else:
                    show_bytes += next_bytes

                inner_loop = True
                while inner_loop:
                    qtty = s.in_waiting

                    if qtty_in_this_chunk == next_bytes:
                        inner_loop = False
                        
                    if qtty:
                        #reviso no leer de mas
                        if (qtty + qtty_in_this_chunk) > next_bytes:
                            qtty = next_bytes - qtty_in_this_chunk

                        read = s.read(qtty)
                        f.write(read)
                        saved_bytes += qtty
                        qtty_in_this_chunk += qtty

    return needed_bytes, saved_bytes, crc


def send_binary_file (serial_instance, file_name):
    s = serial_instance
    sended_bytes = 0
    show_bytes = 0
    rx_bytes = ""
    svd_bytes = ""
    crc = ""
    
    with open(file_name, "rb") as f:
        loop = True

        file_end = False
        s.write("next\n".encode('utf-8'))    #para el primer paquete
        while loop:
            answer = s.readline().decode()        

            #envio el primer next            
            if 'Timeout' in answer:
                loop = False
                
            elif 'Rx:' in answer:
                #Rx: %d Svd: %d CRC: 0x%04x
                loop = False
                s_rx, rx_bytes, s_svd, svd_bytes, s_crc, crc = answer.split(' ')
                
            elif 'n' in answer:
                #envio proximo paquete
                if show_bytes >= 65536:
                    print(sended_bytes)
                    show_bytes = 0
                else:
                    show_bytes += 16
                
                for i in range(16):
                    b = f.read(1)
                    if b == b'':
                        print(f'Error in Sended: {sended_bytes}')
                        break
                    else:
                        sended_bytes += 1
                        s.write(b)

                inner_loop = True
                while inner_loop:
                    answer = s.readline().decode()
                    if '.' in answer:
                        inner_loop = False
                        if sended_bytes >= 524288:
                            s.write("ended\n".encode('utf-8'))
                        else:
                            s.write("next\n".encode('utf-8'))
                        

    return rx_bytes, svd_bytes, crc


                
def flush_serial_buffer (serial_instance):
    s = serial_instance
    qtty = s.in_waiting
    if qtty:
        read = s.read(qtty)
        print("Flushing Buffer")


### MAIN PROGRAM ###
# Set up a loop where users can choose what they'd like to do.
choice = ''
display_os_info()
# para el puerto serie
port = '/dev/ttyUSB0'
velocidad = 115200

try:
    ser = serial.Serial(port, velocidad)
    if (ser != None):
        print ("Serial Port Open on: " + port + " at: " + str(velocidad) + "bps")
        port_open = True
        display_title_bar()
except:
    print ("Serial Port Not Open\n")
    choice = 'q'


while choice != 'q':    
    
    choice = get_user_choice()
    
    # Respond to the user's choice.
    display_title_bar()
    if choice == '1':
        read_all_memory(ser)
    elif choice == '2':
        read_memory_addr(ser)
    # elif choice == '3':
    #     read_silicon_id(ser)
    # elif choice == '4':
    #     read_manufacturer_id(ser)
    # elif choice == '5':
    #     read_protected(ser)
    # elif choice == '6':
    #     blank_check(ser)
    # elif choice == '7':
    #     get_memory_crc(ser)
    # elif choice == '8':
    #     write_all_memory(ser)
    # elif choice == '9':
    #     write_memory_addr(ser)
    # elif choice == '10':
    #     memory_reset(ser)
    # elif choice == '11':
    #     erase_all_memory(ser)
    # elif choice == '12':
    #     crc_from_file()        
    elif choice == 'q':
        print("\nThanks for playing. Bye.")
    else:
        print("\nI didn't understand that choice.\n")
        


        
