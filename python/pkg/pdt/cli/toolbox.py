from __future__ import print_function

import re

import definitions as defs
import click
import time
import pyipmi
import pyipmi.interfaces
import binascii
import struct

from click import echo, style, secho
from click_texttable import Texttable


# ------------------------------------------------------------------------------
def hookDebugger(debugger='gdb'):
    '''debugging helper, hooks debugger to running interpreter process'''

    import os
    pid = os.spawnvp(os.P_NOWAIT,
                     debugger, [debugger, '-q', 'python', str(os.getpid())])

    # give debugger some time to attach to the python process
    import time
    time.sleep( 1 )

    # verify the process' existence (will raise OSError if failed)
    os.waitpid( pid, os.WNOHANG )
    os.kill( pid, 0 )
    return
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
class IntRange(click.types.IntParamType):
    """A parameter that works similar to :data:`click.INT` but restricts
    the value to fit into a range.  The default behavior is to fail if the
    value falls outside the range, but it can also be silently clamped
    between the two edges.

    See :ref:`ranges` for an example.
    """
    name = 'integer range'

    def __init__(self, min=None, max=None, clamp=False):
        self.min = min
        self.max = max
        self.clamp = clamp

    def convert(self, value, param, ctx):
        
        if type(value) == str:
            if value.startswith('0x'):
                base = 16
            elif value.startswith('0o'):
                bae = 8
            elif value.startswith('0b'):
                base = 2   
            else:
                base = 10
            rv = int(value, base)
        else:
            rv = int(value)
        if self.clamp:
            if self.min is not None and rv < self.min:
                return self.min
            if self.max is not None and rv > self.max:
                return self.max
        if self.min is not None and rv < self.min or \
           self.max is not None and rv > self.max:
            if self.min is None:
                self.fail('%s is bigger than the maximum valid value '
                          '%s.' % (rv, self.max), param, ctx)
            elif self.max is None:
                self.fail('%s is smaller than the minimum valid value '
                          '%s.' % (rv, self.min), param, ctx)
            else:
                self.fail('%s is not in the valid range of %s to %s.'
                          % (rv, self.min, self.max), param, ctx)
        return rv

    def __repr__(self):
        return 'IntRange(%r, %r)' % (self.min, self.max)
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def sanitizeConnectionPaths(aConnectionPaths):

    lConnectionList = aConnectionPaths.split(';')
    for i,c in enumerate(lConnectionList):
        if re.match('^\w+://.*', c) is None:
            lConnectionList[i] = 'file://'+c
    return ';'.join(lConnectionList)
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def readSubNodes(aNode, dispatch=True):
    lValues = { n:aNode.getNode(n).read() for n in aNode.getNodes() }

    if dispatch:
        aNode.getClient().dispatch()
    return lValues
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def resetSubNodes(aNode, aValue=0x0, dispatch=True):
    """
    Reset subnodes of aNode to aValue
    """
    lValues = { n:aNode.getNode(n).write(aValue) for n in aNode.getNodes() }
    if dispatch:
        aNode.getClient().dispatch()
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
def hookDebugger(debugger='gdb'):
    '''
    debugging helper, hooks debugger to running interpreter process
    '''

    import os
    pid = os.spawnvp(os.P_NOWAIT,
                     debugger, [debugger, '-q', 'python', str(os.getpid())])

    # give debugger some time to attach to the python process
    import time
    time.sleep( 1 )

    # verify the process' existence (will raise OSError if failed)
    os.waitpid( pid, os.WNOHANG )
    os.kill( pid, 0 )
    return
# ------------------------------------------------------------------------------

# -----------------
def validate_device(ctx, param, value):

    lDevices = ctx.obj.mConnectionManager.getDevices()
    if value not in lDevices:
        raise click.BadParameter(
            'Device must be one of '+
            ', '.join(["'"+lId+"'" for lId in lDevices])
            )
    return value
# -----------------


# -----------------
def complete_device(ctx, args, incomplete):
    lDevices = ctx.obj.mConnectionManager.getDevices()

    return [k for k in lDevices if incomplete in k]
# -----------------


# -----------------
def validate_partition(ctx, param, value):
    
    lMaxParts = ctx.obj.mGenerics['n_part']
    if value < 0 or value > lMaxParts-1:

        raise click.BadParameter(
            'Invalid partition {}. Available partitions: {}'.format(value, range(lMaxParts))
        )
    return value
# -----------------


# ------------------------------------------------------------------------------
def validate_chan(ctx, param, value):
    
    lMaxChans = ctx.obj.mGenerics['n_chan']
    if value < 0 or value > lMaxChans-1:

        raise click.BadParameter(
            'Invalid partition {}. Available partitions: {}'.format(value, range(lMaxChans))
        )
    return value
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
def split(ctx, param, value):
    if value is None:
        return []

    return value.split(',')
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
def __str2int__( value ):
    if value.startswith('0x'):
        base = 16
    elif value.startswith('0o'):
        bae = 8
    elif value.startswith('0b'):
        base = 2   
    else:
        base = 10
    return int(value, base)

def split_ints(ctx, param, value):

    sep = ','
    dash = '-'

    if value is None:
        return []

    numbers = []
    for item in value.split(sep):
        nums = item.split(dash)
        if len(nums) == 1:
            # single entry
            numbers.append(__str2int__(item))
        elif len(nums) == 2:
            # range
            i, j = __str2int__(nums[0]), __str2int__(nums[1])
            if i > j:
                click.ClickException('Invalid interval '+item)
            numbers.extend(xrange(i,j+1))
        else:
           click.ClickException('Malformed option (comma separated list expected): {}'.format(value))

    return numbers
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def printRegTable(aRegs, aHeader=True, sort=True):
    echo  ( formatRegTable(aRegs, aHeader, sort) )
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def formatRegTable(aRegs, aHeader=True, sort=True):
    lRegTable = Texttable(max_width=0)
    lRegTable.set_deco(Texttable.VLINES | Texttable.BORDER | Texttable.HEADER)
    lRegTable.set_chars(['-', '|', '+', '-'])
    if aHeader:
        lRegTable.header( ['name', 'value'] )

    lRegs = sorted(aRegs) if sort else aRegs
    for k in lRegs:
        lRegTable.add_row( [str(k), hex(aRegs[k])] )
        
    return lRegTable.draw()
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
def printDictTable(aDict, aHdr=True, aSort=True, aFmtr=None):
    echo  ( formatDictTable(aDict, aHdr, aSort, aFmtr) )
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def formatDictTable(aDict, aHdr=True, aSort=True, aFmtr=str):
    lDictTable = Texttable(max_width=0)
    lDictTable.set_deco(Texttable.VLINES | Texttable.BORDER | Texttable.HEADER)
    lDictTable.set_chars(['-', '|', '+', '-'])
    if aHdr:
        lDictTable.header( ['name', 'value'] )

    for k in (sorted(aDict) if aSort else aDict):
        v = aDict[k]
        lDictTable.add_row( [str(k), aFmtr(v) if aFmtr else v])
        
    return lDictTable.draw()
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def collateTables(t1, t2):
    l1 = t1.split('\n')
    l2 = t2.split('\n')

    col1 = max([len(escape_ansi(l)) for l in l1])
    col2 = max([len(escape_ansi(l)) for l in l2])

    nrows = max(len(l1), len(l2));

    l1 += [''] * (nrows - len(l1))
    l2 += [''] * (nrows - len(l2))
    fmt = '\'{:<%d}\' \'{:<%d}\'' % (col1, col2)
    for c1,c2 in zip(l1, l2):
        print (c1 + ' '*(col1-len(escape_ansi(c1))), '  ' ,c2 + ' '*(col2-len(escape_ansi(c2))))
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
kReEscapeAnsi = re.compile(r'(\x9B|\x1B\[)[0-?]*[ -/]*[@-~]')

def escape_ansi(line):
    return kReEscapeAnsi.sub('', line)
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
def formatTStamp( aRawTStamp ):
    ts = int(aRawTStamp[0]) + int((aRawTStamp[1]) << 32)
    
    lSubSec = ts % defs.kSPSClockInHz
    lSecFromEpoch = ts / defs.kSPSClockInHz

    return time.strftime("%a, %d %b %Y %H:%M:%S +0000", time.localtime(lSecFromEpoch))
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def tstamp2int( aRawTStamp ):
    return int(aRawTStamp[0]) + int((aRawTStamp[1]) << 32)
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def printCounters( aTopNode, aSubNodes, aNumCtrs=0x10, aTitle='Cmd', aLegend=defs.kCommandNames ):

    lBlocks = []

    # try:
    #     aTopNode.getClient().dispatch()
    # except uhal.exception as e:
    #     for b in lBlocks:
    #         print (b.valid())
    #     raise SystemExit(0)
    
    for lKey in aSubNodes:
        try:
            ctrs = aTopNode.getNode(lKey).readBlock(aNumCtrs)
            aTopNode.getClient().dispatch()
            lBlocks.append(ctrs.value())

        except uhal.exception as e:
            print ('Failed to read ', lKey)
            lBlocks.append([None]*aNumCtrs)

    # Just a bit of math
    lCellWidth = 12
    kCellFmt = ' {{:^{}}} '.format(lCellWidth)
    kTitleCellFmt = ' {{:^{}}} '.format((lCellWidth+1)*2+1)

    lLineLen = (lCellWidth+2+1)*(len(aSubNodes)*2+1)+1

    # Build the title
    lLine = [kCellFmt.format('')]+[style(kTitleCellFmt.format(aSubNodes[lName]), fg='green') for lName in aSubNodes]
 
    lTitle = '|'.join(['']+lLine+[''])
    echo ( '-'*lLineLen)
    # print ( '-'*len(lTitle))
    echo ( lTitle )

    # Build the title
    lLine = [aTitle] +( ['cnts', 'hex' ]*len(aSubNodes) )
    lHdr = '|'.join(['']+[kCellFmt.format(lCell) for lCell in lLine]+[''])
    print ( '-'*lLineLen)
    print ( lHdr )
    print ( '-'*lLineLen)

    for lId in xrange(aNumCtrs):

        lLine = [ (aLegend.get(lId,hex(lId))) ]
        for lBlock in lBlocks:
            lLine += [lBlock[lId],hex(lBlock[lId])] if lBlock[lId] is not None else ['fail']*2
        print( '|'.join(['']+[kCellFmt.format(lCell) for lCell in lLine]+['']))
    print ( '-'*lLineLen)
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def establishIPMIConnectionToAMC(mch_ip_adr,amc_slot):
    amc_ipmb_addresses= [0x72, 0x74, 0x76, 0x78, 0x7a, 0x7c, 0x7e, 0x80, 0x82, 0x84, 0x86, 0x88]
    amc_ipmb_adr = amc_ipmb_addresses[amc_slot-1]
    
    interface = pyipmi.interfaces.create_interface('ipmitool', interface_type='lan')
    connection = pyipmi.create_connection(interface)

    connection.target = pyipmi.Target(amc_ipmb_adr)
    connection.target.set_routing([(0x81,0x20,0),(0x20,0x82,7),(0x20,amc_ipmb_adr,None)])
    connection.session.set_session_type_rmcp(mch_ip_adr, port=623)
    connection.session.set_auth_type_user('', '')
    connection.session.establish()
    return connection
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def readRegOverIPMI(ipmi_connection, reg):
    raw_read_cmd = b'\x00\x02\x4B\x01\x01'
    cmd = raw_read_cmd+struct.pack("B", reg)
    
    max_attempts=10
    read_attempts=0

    while True:
        if read_attempts > max_attempts:
            raise click.ClickException("Failed to read value of reg {} after {} attempts".format(hex(reg), max_attempts))
        read_cmd_result = []
        result = ipmi_connection.raw_command(0x00, 0x30, cmd)
        for char in result:
            read_cmd_result.append(ord(char))
        if read_cmd_result[1] == 1 and read_cmd_result[2] == 1:
            return read_cmd_result[3]
        else:
            read_attempts += 1
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def writeRegOverIPMI(ipmi_connection, reg, data):
    raw_write_cmd = b'\x00\x02\x4B\x02\x01'
    cmd = raw_write_cmd+struct.pack("B", reg)+struct.pack("B", data)
    
    max_attempts=10
    write_attempts=0

    while True:
        if write_attempts > max_attempts:
            raise click.ClickException("Failed to write value of reg {} after {} attempts".format(hex(reg), max_attempts))
        write_cmd_result = []
        result = ipmi_connection.raw_command(0x00, 0x30, cmd)
        for char in result:
            write_cmd_result.append(ord(char))
        if write_cmd_result[1] == 2 and write_cmd_result[2] == 1:
            return
        else:
            write_attempts += 1
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def applyCrossbarTxConfig(ipmi_connection, tx_enable_flag):
    tx_control_ctrl_reg_start = 0x20
    
    for i in range(16):
        reg_adr = tx_control_ctrl_reg_start+i 

        # TX Basic Control Register flags:
        # [6] TX CTL SELECT - 0: PE and output level control is derived from common lookup table
        #                     1: PE and output level control is derived from per port drive control registers
        # [5:4] TX EN[1:0]  - 00: TX disabled, lowest power state
        #                     01: TX standby
        #                     10: TX squelched
        #                     11: TX enabled
        # [3] Reserved      - Set to 0
        # [2:1] PE[2:0]     - If TX CTL SELECT = 0,
        #                       000: Table Entry 0
        #                       001: Table Entry 1
        #                       010: Table Entry 2
        #                       011: Table Entry 3
        #                       100: Table Entry 4
        #                       101: Table Entry 5
        #                       110: Table Entry 6
        #                       111: Table Entry 7
        #                   - If TX CTL SELECT = 1, PE[2:0] are ignored
        tx_state = 0b0110000 if tx_enable_flag & (1 << i) else 0b0000000
        #print("tx state for output {} at adr {}: ".format(i,hex(reg_adr)) +hex(tx_state))
        writeRegOverIPMI(ipmi_connection, reg_adr, tx_state)
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def applyCrossbarXPTMapConfig(ipmi_connection, xpt_map, map_number):
    xpt_reg_values=[]
    for i in range(2,18,2):
        map_value_str = xpt_map[i-2:i]
        map_value=int(map_value_str, 16)
        reg_value_lo = int('{:08b}'.format(map_value)[4:], 2) << 4
        reg_value_hi = int('{:08b}'.format(map_value)[:4], 2)
        reg_value = reg_value_lo | reg_value_hi
        xpt_reg_values.append(reg_value)

    xpt_map_reg_adrs_start=[0x90, 0x98]

    for i in range(len(xpt_reg_values)):
        reg_adr = xpt_map_reg_adrs_start[map_number]+i
        reg_value = xpt_reg_values[i]
        writeRegOverIPMI(ipmi_connection, reg_adr, reg_value)
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def readCrossbarXPTMapConfig(ipmi_connection, map_number):
    xpt_map=[]

    xpt_map_reg_adrs_start=[0x90, 0x98]

    for i in range(8):
        reg_adr = xpt_map_reg_adrs_start[map_number]+i
        reg_value = readRegOverIPMI(ipmi_connection, reg_adr)
        reg_value_lo = reg_value & 0x0f
        reg_value_hi = (reg_value >> 4) & 0x0f
        xpt_map.append(reg_value_lo)
        xpt_map.append(reg_value_hi)
    return xpt_map

def readCrossbarTxConfig(ipmi_connection):
    tx_states=[]

    tx_control_ctrl_reg_start = 0x20

    for i in range(16):
        reg_adr = tx_control_ctrl_reg_start+i
        reg_value = readRegOverIPMI(ipmi_connection, reg_adr)
        tx_states.append(reg_value)
    return tx_states
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def formatCrossbarConfigTable(map_0, map_1, tx, active_map):

    map_0_colour='green'
    map_1_colour='white'
    if active_map == 1:
        map_0_colour='white'
        map_1_colour='green'

    configTable = Texttable(max_width=0)
    configTable.set_deco(Texttable.VLINES | Texttable.BORDER | Texttable.HEADER)
    configTable.set_cols_align(["l", "l", "l", "l"])
    configTable.set_chars(['-', '|', '+', '-'])
    configTable.header( ['Output', style('Map 0', fg=map_0_colour), style('Map 1', fg=map_1_colour), 'Tx state'] )

    for i in range(len(map_0)):
        tx_state = (tx[i] & 0b0110000) >> 4 
        tx_state_names = ["Disabled", "Standby", "Squelched", "Enabled"]
        tx_state_colours=['red', 'white', 'blue', 'green']
        configTable.add_row( [i, style(str(map_0[i]), fg=map_0_colour), style(str(map_1[i]), fg=map_1_colour), style(tx_state_names[tx_state], fg=tx_state_colours[tx_state])] )
        
    return configTable.draw()
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
def readGPIOPortOverIPMI(ipmi_connection, port):
    raw_gpio_cmd = b'\x01'
    mode=b'\x00'
    cmd = raw_gpio_cmd+struct.pack("B", port)+mode
    
    cmd_result = []
    result = ipmi_connection.raw_command(0x00, 0x30, cmd)

    for char in result:
        cmd_result.append(ord(char))
    
    # first 4 bytes contain the pin directions    
    port_directions_flag=0b0
    for i in range(4,0,-1):
        port_directions_flag = (port_directions_flag << 8) | cmd_result[i]

    # second 4 bytes contain the pin states
    port_states_flag=0b0
    for i in range(8,4,-1):
        port_states_flag = (port_states_flag << 8) | cmd_result[i]

    portTable = Texttable(max_width=0)
    portTable.set_deco(Texttable.VLINES | Texttable.BORDER | Texttable.HEADER)
    portTable.set_cols_align(["l", "l", "l"])
    portTable.set_chars(['-', '|', '+', '-'])
    portTable.header( ['Pin', 'Direction', 'State'] )

    for i in range(32):
        pin_dir = 'In'
        pin_dir_colour = 'yellow'
        if port_directions_flag & (0x1 << i):
            pin_dir = 'Out'
            pin_dir_colour = 'green'
        
        pin_state = 'Low'
        pin_state_colour = 'blue'
        if port_states_flag & (0x1 << i):
            pin_state = 'High'
            pin_state_colour = 'red'

        portTable.add_row( [i, style(pin_dir, fg=pin_dir_colour), style(pin_state, fg=pin_state_colour)] )
    
    print("Port {} pins".format(port))
    return portTable.draw()
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def configureGPIOPortOverIPMI(ipmi_connection, port, mode, pin, value=-1):
    
    if mode < 1 or mode > 2:
        raise click.ClickException("Valid configuring modes are 1 or 2")

    raw_gpio_cmd = b'\x01'
    cmd = raw_gpio_cmd + struct.pack("B", port) + struct.pack("B", mode) + struct.pack("B", pin)
    
    if mode==2 and value >= 0:
        cmd = cmd+struct.pack("B", value)

    cmd_attempts=0
    max_attempts=10
    while True:
        if cmd_attempts > max_attempts:
            raise click.ClickException("Failed to configure port {} pin {} after {} attempts".format(hex(port), hex(pin), max_attempts))
        cmd_result = []
        result = ipmi_connection.raw_command(0x00, 0x30, cmd)
        for char in result:
            cmd_result.append(ord(char))
        if cmd_result[1] == 0 and mode == 0x1:
            return
        elif cmd_result[1] == 1 and mode == 0x2:
            if value < 0:
                return
            else:
                if cmd_result[2] == value:
                    return
                else:
                    echo ("error configured pin state {}, does not match requested {}".format(cmd_result[2], value))
                    cmd_attempts += 1
        else:
            cmd_attempts += 1
# ------------------------------------------------------------------------------


#sfp related functions
# ------------------------------------------------------------------------------
def GetCalibSlopeFromSFP(lSFP,index):
    # calib slope addresses, each slope is 16 bits
    slope_adr = [ [0x4C, 0x4D], #laser current
                  [0x50, 0x51], #tx_pwr
                  [0x54, 0x55], #temp
                  [0x58, 0x59] ]#voltage supply
    slope_whole = lSFP.readI2C(0x51,slope_adr[index][0])
    slope_decimal = lSFP.readI2C(0x51,slope_adr[index][1]) / 256.0
    return slope_whole+slope_decimal
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
def GetCalibOffsetFromSFP(lSFP,index):
    # calib slope addresses, each slope is 16 bits
    slope_adr = [ [0x4E, 0x4F],  #laser current
                  [0x52, 0x53],  #tx_pwr
                  [0x56, 0x57],  #temp
                  [0x5A, 0x5B] ] #voltage supply
    
    offset = lSFP.readI2C(0x51,slope_adr[index][0])
    #first bit corresponds to sign
    if (offset & (1 << (8 - 1))) != 0:
        offset = offset - (1 << 8)
    return (offset << 8) | lSFP.readI2C(0x51,slope_adr[index][1])
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def GetSFPTemperatureRaw(lSFP):
    temp_raw_whole = lSFP.readI2C(0x51,0x60)
    #first bit corresponds to sign
    if (temp_raw_whole & (1 << (8 - 1))) != 0:
        temp_raw_whole = temp_raw_whole - (1 << 8)
    temp_raw_decimal = lSFP.readI2C(0x51,0x61) / 256.0
    return temp_raw_whole + temp_raw_decimal
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def GetSFPTemperatureCalibrated(lSFP):
    temp_raw = GetSFPTemperatureRaw(lSFP)
    slope = GetCalibSlopeFromSFP(lSFP,2)
    offset = GetCalibOffsetFromSFP(lSFP,2)
    return (temp_raw*slope) + offset
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def GetSFPVoltageRaw(lSFP):
    return (lSFP.readI2C(0x51,0x62) << 8) | lSFP.readI2C(0x51,0x63)

def GetSFPVoltageCalibrated(lSFP):
    voltage_raw = GetSFPVoltageRaw(lSFP)
    slope = GetCalibSlopeFromSFP(lSFP,3)
    offset = GetCalibOffsetFromSFP(lSFP,3)
    return ((voltage_raw*slope)+offset)*1e-4
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def GetSFPRxPowerRaw(lSFP):
    return (lSFP.readI2C(0x51,0x68) << 8) | lSFP.readI2C(0x51,0x69)
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def GetSFPRxPowerCalibrated(lSFP):
    rx_pwr_raw = GetSFPRxPowerRaw(lSFP)

    # rx power calib constants, 5 4-byte (floats) parameters
    rx_pars_adr = [[0x48, 0x49, 0x4A, 0x4B], [0x44, 0x45, 0x46, 0x47], [0x40, 0x41, 0x42, 0x43], [0x3C, 0x3D, 0x3E, 0x3F], [0x38, 0x39, 0x3A, 0x3B]]
    rx_pars = []
    for par_adr in rx_pars_adr:
        par=0
        for adr in par_adr:
            val = lSFP.readI2C(0x51,adr)
            par = (par << 8) | val
        #print(format(par, '032b'))
        par = struct.unpack("<f", struct.pack("<i", par))[0] # convert the 32 bits to a float
        #print("float: {}".format(par))
        rx_pars.append(par)

    rx_pars_counter=0
    rx_pwr_calib=0
    for par in rx_pars:
        rx_pwr_calib = rx_pwr_calib + (par*pow(rx_pwr_raw,rx_pars_counter))
        rx_pars_counter=rx_pars_counter+1
    return rx_pwr_calib*0.1
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def GetSFPTxPowerRaw(lSFP):
    return (lSFP.readI2C(0x51,0x66) << 8) | lSFP.readI2C(0x51,0x67)
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def GetSFPTxPowerCalibrated(lSFP):
    tx_power_raw = GetSFPTxPowerRaw(lSFP)
    slope = GetCalibSlopeFromSFP(lSFP,1)
    offset = GetCalibOffsetFromSFP(lSFP,1)
    return ((tx_power_raw*slope) + offset)*0.1
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def GetSFPCurrentRaw(lSFP):
    return (lSFP.readI2C(0x51,0x64) << 8) | lSFP.readI2C(0x51,0x65)
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def GetSFPCurrentCalibrated(lSFP):
    current_raw = GetSFPCurrentRaw(lSFP)
    slope = GetCalibSlopeFromSFP(lSFP,0)
    offset = GetCalibOffsetFromSFP(lSFP,0)
    return ((current_raw*slope) + offset)*0.002
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def PrintSFPStatus(lSFP, sfpNumber):
    sfpTable = Texttable(max_width=0)
    sfpTable.set_deco(Texttable.VLINES | Texttable.BORDER | Texttable.HEADER)
    sfpTable.set_cols_align(["c", "c"])
    sfpTable.set_chars(['-', '|', '+', '-'])

    # SFP number
    sfpTable.add_row( ['SFP #', sfpNumber] )

    # Is the sfp reachable?
    if not lSFP.ping(0x50):
        if not lSFP.ping(0x51):
            # SFP should present address 0x50, or 0x51, or both, but not none
            return sfpTable.draw()
        else:
            # If only address 0x51 is visible, SFP requires special I2C change to swap between memory areas
            secho("SFP in mode 0x51, special I2C address change not implemented", fg='red')
            return sfpTable.draw()
    
    # Vendor name
    vendor=''
    for adr in range(0x14, 0x23):
        char = lSFP.readI2C(0x50,adr)
        vendor=vendor+chr(char)
    sfpTable.add_row( ['Vendor', vendor] )
     
    # Vendor part number
    pn=''
    for adr in range(0x28, 0x37):
        char = lSFP.readI2C(0x50,adr)
        pn=pn+chr(char)
    sfpTable.add_row( ['Part number', pn] )

    # Bit 6 of byte 5C tells us whether the SFP supports digital diagnostic monitoring (DDM)
    ddm_info = lSFP.readI2C(0x50,0x5C)
    mon_diag_mask = 0b01000000
    mon_diag_enabled = ddm_info&mon_diag_mask
    
    if not mon_diag_enabled:
        return sfpTable.draw()

    # Bit 2 of byte 5C tells us whether special I2C address change operations are needed to access the DDM area
    adr_change_mask = 0b00000100
    adr_change_needed = ddm_info&adr_change_mask

    if adr_change_needed:
        secho("Special I2C address change not supported", fg='red')
        return sfpTable.draw()

    temp_calib = GetSFPTemperatureCalibrated(lSFP)
    sfpTable.add_row( ['Temperature', "{:.1f} C".format(temp_calib)] )
        
    voltage_calib = GetSFPVoltageCalibrated(lSFP)
    sfpTable.add_row( ['Supply voltage', "{:.1f} V".format(voltage_calib)] )

    rx_power_calib = GetSFPRxPowerCalibrated(lSFP)
    sfpTable.add_row( ['Rx power', "{:.1f} uW".format(rx_power_calib)] )

    tx_power_calib = GetSFPTxPowerCalibrated(lSFP)
    sfpTable.add_row( ['Tx power', "{:.1f} uW".format(tx_power_calib)] )

    current_calib = GetSFPCurrentCalibrated(lSFP)
    sfpTable.add_row( ['Laser bias current',  "{:.1f} mA".format(current_calib)] )

    # Bit 6 of byte 5d tells us whether the soft tx control is implemented in this sfp
    enhanced_options = lSFP.readI2C(0x50,0x5d)
    soft_tx_control_enabled_mask = 0b01000000
    soft_tx_control_enabled = enhanced_options&soft_tx_control_enabled_mask

    # Get optional status/control bits
    opt_status_ctrl_byte = lSFP.readI2C(0x51,0x6e) 

    if soft_tx_control_enabled:
        sfpTable.add_row( ['Tx disbale reg supported',  'True'] )

        # Bit 6 tells us the state of the soft tx_disble register
        tx_diable_reg_state = 1 if opt_status_ctrl_byte & (1 << 6) != 0 else 0
        sfpTable.add_row( ['Tx disable reg', "{}".format(tx_diable_reg_state)] )
    else:
        sfpTable.add_row( ['Tx disbale reg supported',  'False'] )

    # Bit 7 tells us the state of tx_disble pin
    tx_diable_pin_state = 1 if opt_status_ctrl_byte & (1 << 7) != 0 else 0
    sfpTable.add_row( ['Tx disable pin', "{}".format(tx_diable_pin_state)] )

    return sfpTable.draw()
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
def ControlSFPTxEnable(lSFP,on,sfpNumber):

    # Is the sfp reachable?
    if not lSFP.ping(0x50):
        if not lSFP.ping(0x51):
            # SFP should present address 0x50, or 0x51, or both, but not none
            secho("SFP not available", fg='red')
            return
        else:
            # If only address 0x51 is visible, SFP requires special I2C change to swap between memory areas
            secho("SFP in mode 0x51, special I2C address change not supported", fg='red')
            return

    # Bit 6 of byte 5C tells us whether the SFP supports digital diagnostic monitoring (DDM)
    ddm_info = lSFP.readI2C(0x50,0x5C)
    mon_diag_mask = 0b01000000
    mon_diag_enabled = ddm_info&mon_diag_mask
    
    if not mon_diag_enabled:
        secho("SFP does not support DDM", fg='red')
        return

    # Bit 2 of byte 5C tells us whether special I2C address change operations are needed to access the DDM area
    adr_change_mask = 0b00000100
    adr_change_needed = ddm_info&adr_change_mask

    if adr_change_needed:
        secho("Special I2C address change not supported", fg='red')
        return

    enhanced_options = lSFP.readI2C(0x50,0x5d)

    # Bit 6 of byte 0x5d tells us whether the soft tx control is implemented in this sfp
    soft_tx_control_enabled_mask = 0b01000000
    soft_tx_control_enabled = enhanced_options&soft_tx_control_enabled_mask

    if (not soft_tx_control_enabled):
        secho("WARNING Soft tx disable not supported by this SFP \n", fg='red')

    # Get optional status/control bits
    opt_status_ctrl_byte = lSFP.readI2C(0x51,0x6e)    

    # Bit 6 of byte 0x6e controls the soft tx_disable
    if (on):
        new_opt_status_ctrl_byte = opt_status_ctrl_byte & ~(1 << 6)        
    else:
        new_opt_status_ctrl_byte = opt_status_ctrl_byte | 1 << 6;
    lSFP.writeI2C(0x51,0x6e,new_opt_status_ctrl_byte)
    
    time.sleep(0.2)

    echo(PrintSFPStatus(lSFP, sfpNumber) )
# ------------------------------------------------------------------------------
