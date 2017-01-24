import pexpect, os
from datetime import datetime

ADCPATH = './ADC.elf'

class ADCRunner:
    @staticmethod
    def date2num()->[]:
        """
        Utility function which returns the current time in the correct format for passing to ADC.elf
        """
        now = datetime.utcnow()
        return [now.year,now.month,now.day,now.hour,now.minute,now.second]


    def process_output(self,values: str) -> ():
        """
        Processes the response of ADC.elf and returns a tuple containing the magnetic moment and quaternion estimates.
        :param values:
        :return:
        """
        floats = [float(f) for f in values.split(',') if values and f]
        # if len(floats) == 7:
        mag = (floats[0], floats[1], floats[2])
        quat = (floats[3], floats[4], floats[5], floats[6])
        return (mag, quat)

    def format_input(self,invals: {}) -> bytes:
        """
        Takes measurements of magnetic moment, euler angle, sun position, epoch, lla, and sun sensor status and prepares them to be passed to ADC.elf.
        :param invals:
        :return:
        """
        concat = lambda s, f: ''.join([f % x for x in s])
        retval = ''
        retval += concat(invals['mag_meas'], '%3.2f,')
        retval += concat(invals['euler_angle'], '%3.2f,')
        retval += concat(invals['sun_meas'], '%3.2f,')
        retval += concat(invals['epoch'], '%02.0f,')
        retval += concat(invals['lla'], '%3.2f,')
        retval += concat([invals['s_flag']], '%1.0f,')
        retval = retval[:-1] #remove the trailing comma
        retval += os.linesep
        return retval.encode('utf-8')

    def page(self,input: {}) -> ():
        """
        Gives ADC.elf the current set of sensor measurements and returns the estimated quaternion and magnetic moment vector.
        :param input:
        :return:
        """
        formatted = self.format_input(input)
        self.pid.write(formatted)
        response = self.pid.readline()

        return self.process_output(response.decode())

    def __init__(self):
        """Begins the long-running process ADC.elf"""
        self.pid = pexpect.spawn(ADCPATH)
        self.pid.setecho(False)

    def exit(self):
        """
        Sends SIGTERM to ADC.elf.
        :return:
        """
        self.pid.terminate()