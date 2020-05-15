from __future__ import print_function

# Python imports
import uhal
import click
import click_didyoumean
import time
import collections
import math
import struct
import pdt

import pdt.cli.toolbox as toolbox
import pdt.common.definitions as defs

from click import echo, style, secho
from os.path import join, expandvars, basename
from pdt.core import SI534xSlave, I2CExpanderSlave, DACSlave

from pdt.common.definitions import kBoardSim, kBoardFMC, kBoardPC059, kBoardMicrozed, kBoardTLU, kBoardFIB
from pdt.common.definitions import kCarrierEnclustraA35, kCarrierKC705, kCarrierMicrozed, kCarrierAFC
from pdt.common.definitions import kDesingMaster, kDesignOuroboros, kDesignOuroborosSim, kDesignEndpoint, kDesingFanout
from pdt.common.definitions import kBoardNamelMap, kCarrierNamelMap, kDesignNameMap

kFMCRev1 = 1
kFMCRev2 = 2
kPC059Rev1 = 3
kPC059FanoutHDMI = 4
kPC059FanoutSFP = 5
kTLURev1 = 6

kClockConfigMap = {
    kFMCRev1: "SI5344/PDTS0000.txt",
    kFMCRev2: "SI5344/PDTS0003.txt",
    kPC059Rev1: "SI5345/PDTS0005.txt",
    kPC059FanoutHDMI: "devel/PDTS_PC059_FANOUT.txt",
    kPC059FanoutSFP: "wr/FANOUT_PLL_WIDEBW_SFPIN.txt",
    kTLURev1: "wr/TLU_EXTCLK_10MHZ_NOZDM.txt"
}

kUIDRevisionMap = {
    0xd880395e720b: kFMCRev1,
    0xd880395e501a: kFMCRev1,
    0xd880395e50b8: kFMCRev1,
    0xd880395e501b: kFMCRev1,
    0xd880395e7201: kFMCRev1,
    0xd880395e4fcc: kFMCRev1,
    0xd880395e5069: kFMCRev1,
    0xd880395e7206: kFMCRev1,
    0xd880395e1c86: kFMCRev2,
    0xd880395e2630: kFMCRev2,
    0xd880395e262b: kFMCRev2,
    0xd880395e2b38: kFMCRev2,
    0xd880395e1a6a: kFMCRev2,
    0xd880395e36ae: kFMCRev2,
    0xd880395e2b2e: kFMCRev2,
    0xd880395e2b33: kFMCRev2,
    0xd880395e1c81: kFMCRev2,
    0xd88039d980cf: kPC059Rev1,
    0xd88039d98adf: kPC059Rev1,
    0xd88039d92491: kPC059Rev1,
    0xd88039d9248e: kPC059Rev1,
    0xd88039d98ae9: kPC059Rev1,
    0xd88039d92498: kPC059Rev1,
}

# ------------------------------------------------------------------------------
#    __  ___         __         
#   /  |/  /__ ____ / /____ ____
#  / /|_/ / _ `(_-</ __/ -_) __/
# /_/  /_/\_,_/___/\__/\__/_/   
                        

@click.group('io', invoke_without_command=True)
@click.pass_obj
@click.argument('device', callback=toolbox.validate_device)
def io(obj, device):
    '''
    Timing master commands.

    DEVICE: uhal device identifier
    '''
    lDevice = obj.mConnectionManager.getDevice(str(device))
    if obj.mTimeout:
        lDevice.setTimeoutPeriod(obj.mTimeout)
        
    echo('Created device ' + click.style(lDevice.id(), fg='blue'))

    lBoardInfo = toolbox.readSubNodes(lDevice.getNode('io.config'), False)
    lDevice.dispatch()


    echo("Design '{}' on board '{}' on carrier '{}'".format(
        style(kDesignNameMap[lBoardInfo['design_type'].value()], fg='blue'),
        style(kBoardNamelMap[lBoardInfo['board_type'].value()], fg='blue'),
        style(kCarrierNamelMap[lBoardInfo['carrier_type'].value()], fg='blue')
    ))

    obj.mDevice = lDevice
    
    obj.mBoardType = lBoardInfo['board_type'].value()
    obj.mCarrierType = lBoardInfo['carrier_type'].value()
    obj.mDesignType = lBoardInfo['design_type'].value()
    
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@io.command('reset', short_help="Perform a hard reset on the timing master.")
@click.option('--soft', '-s', is_flag=True, default=False, help='Soft reset i.e. skip the clock chip configuration.')
@click.option('--fanout-mode', 'fanout', type=click.IntRange(0, 1), default=0, help='Configures the board in fanout mode (pc059 only)')
@click.option('--force-pll-cfg', 'forcepllcfg', type=click.Path(exists=True))
@click.pass_obj
@click.pass_context
def reset(ctx, obj, soft, fanout, forcepllcfg):
    '''
    Perform a hard reset on the timing master, including

    \b
    - ipbus registers
    - i2c buses
    - pll and pll configuration

    \b
    Fanout mode:
    0 = local master
    1 = sfp
    '''

    echo('Resetting ' + click.style(obj.mDevice.id(), fg='blue'))

    lDevice = obj.mDevice
    lBoardType = obj.mBoardType
    lCarrierType = obj.mCarrierType
    lDesignType = obj.mDesignType

    lIO = lDevice.getNode('io')

    if ( lBoardType == kBoardPC059 and fanout ):
        secho("Fanout mode enabled", fg='green')

    if not (soft or lBoardType == kBoardSim):
        
        time.sleep(1)
        
        # PLL and I@C reset 
        lIO.getNode('csr.ctrl.pll_rst').write(0x1)
        if lBoardType == kBoardPC059:
            lIO.getNode('csr.ctrl.rst_i2c').write(0x1)
            lIO.getNode('csr.ctrl.rst_i2cmux').write(0x1)


        elif lBoardType == kBoardTLU:
            lIO.getNode('csr.ctrl.rst_i2c').write(0x1)

        lDevice.dispatch()

        lIO.getNode('csr.ctrl.pll_rst').write(0x0)
        if lBoardType == kBoardPC059:
            lIO.getNode('csr.ctrl.rst_i2c').write(0x0)
            lIO.getNode('csr.ctrl.rst_i2cmux').write(0x0)
        
        elif lBoardType == kBoardTLU:
            lIO.getNode('csr.ctrl.rst_i2c').write(0x0)

        lDevice.dispatch()


        # Detect the on-board eprom and read the board UID
        if lBoardType in [kBoardPC059, kBoardTLU]:
            lUID = lIO.getNode('i2c')
        else:
            lUID = lIO.getNode('uid_i2c')

        echo('UID I2C Slaves')
        for lSlave in lUID.getSlaves():
            echo("  {}: {}".format(lSlave, hex(lUID.getSlaveAddress(lSlave))))

        if (
            lBoardType == kBoardTLU or
            lBoardType == kBoardPC059 or
            (lBoardType == kBoardFMC and lCarrierType == kCarrierEnclustraA35)
            ):

            try:
                # Wake up the switch
                lUID.getSlave('AX3_Switch').writeI2C(0x01, 0x7f)
            except RuntimeError:
                pass

            x = lUID.getSlave('AX3_Switch').readI2C(0x01)
            echo("I2C enable lines: {}".format(x))
        elif lCarrierType == kCarrierKC705:
            lUID.getSlave('KC705_Switch').writeI2CPrimitive([0x10])
            # x = lUID.getSlave('KC705_Switch').readI2CPrimitive(1)
            echo("KC705 I2C switch enabled (hopefully)".format())
        else:
            click.ClickException("Unknown board kind {}".format(lBoardType))


        # 
        # If not a TLU, read the unique ID from the prom 
        # if lBoardType != kBoardTLU:

        lPROMSlave = 'FMC_UID_PROM'
        if lBoardType == kBoardTLU:
            lPROMSlave =  'UID_PROM'
        elif lCarrierType == kCarrierAFC:
            lPROMSlave = 'AFC_FMC_UID_PROM'

        lValues = lUID.getSlave(lPROMSlave).readI2CArray(0xfa, 6)
        lUniqueID = 0x0
        for lVal in lValues:
            lUniqueID = ( lUniqueID << 8 ) | lVal
        echo("Timing Board PROM UID: "+style(hex(lUniqueID), fg="blue"))

        if lBoardType != kBoardTLU:
            # Ensure that the board is known to the revision DB
            try:
                lRevision = kUIDRevisionMap[lUniqueID]
            except KeyError:
                raise click.ClickException("No revision associated to UID "+hex(lUniqueID))

        # Access the clock chip
        if lBoardType in [kBoardPC059, kBoardTLU]:
            lI2CBusNode = lDevice.getNode("io.i2c")
            lSIChip = SI534xSlave(lI2CBusNode, lI2CBusNode.getSlave('SI5345').getI2CAddress())
        else:
            lSIChip = lIO.getNode('pll_i2c')
        lSIVersion = lSIChip.readDeviceVersion()
        echo("PLL version : "+style(hex(lSIVersion), fg='blue'))

        # Ensure that the board revision has a registered clock config
        if forcepllcfg is not None:
            lFullClockConfigPath = forcepllcfg
            echo("Using PLL Clock configuration file: "+style(basename(lFullClockConfigPath), fg='green') )

        else:
            if lBoardType == kBoardTLU:
                lClockConfigPath = kClockConfigMap[kTLURev1]
            elif lDesignType == kDesingFanout and fanout in [0]:
                secho("Overriding clock config - fanout mode", fg='green')
                lClockConfigPath = kClockConfigMap[kPC059FanoutHDMI if fanout == 1 else kPC059FanoutSFP]
            else:
                try:
                    lClockConfigPath = kClockConfigMap[lRevision]    
                except KeyError:
                    raise ClickException("Board revision " << lRevision << " has no associated clock configuration")


            echo("PLL Clock configuration file: "+style(lClockConfigPath, fg='green') )

            # Configure the clock chip
            lFullClockConfigPath = expandvars(join('${PDT_TESTS}/etc/clock', lClockConfigPath))

        lSIChip.configure(lFullClockConfigPath)
        echo("SI3545 configuration id: {}".format(style(lSIChip.readConfigID(), fg='green')))


        if lDesignType == kDesingFanout:
            lDevice.getNode('switch.csr.ctrl.master_src').write(fanout)
            lIO.getNode('csr.ctrl.mux').write(0)
            lDevice.dispatch()

        ctx.invoke(clkstatus)

        if lBoardType == kBoardFMC:
            lDevice.getNode("io.csr.ctrl.sfp_tx_dis").write(0)
            lDevice.dispatch()
        elif lBoardType == kBoardPC059:
            lI2CBusNode = lDevice.getNode("io.i2c")
            lSFPExp = I2CExpanderSlave(lI2CBusNode, lI2CBusNode.getSlave('SFPExpander').getI2CAddress())

            # Set invert registers to default for both banks
            lSFPExp.setInversion(0, 0x00)
            lSFPExp.setInversion(1, 0x00)

            # BAnk 0 input, bank 1 output
            lSFPExp.setIO(0, 0x00)
            lSFPExp.setIO(1, 0xff)

            # Bank 0 - enable all SFPGs (enable low)
            lSFPExp.setOutputs(0, 0x00)
            secho("SFPs 0-7 enabled", fg='cyan')
        elif lBoardType == kBoardTLU:

            lIC6 = I2CExpanderSlave(lI2CBusNode, lI2CBusNode.getSlave('Expander1').getI2CAddress())
            lIC7 = I2CExpanderSlave(lI2CBusNode, lI2CBusNode.getSlave('Expander2').getI2CAddress())

            # Bank 0
            lIC6.setInversion(0, 0x00)
            lIC6.setIO(0, 0x00)
            lIC6.setOutputs(0, 0x00)

            # Bank 1
            lIC6.setInversion(1, 0x00)
            lIC6.setIO(1, 0x00)
            lIC6.setOutputs(1, 0x88)


            # Bank 0
            lIC7.setInversion(0, 0x00)
            lIC7.setIO(0, 0x00)
            lIC7.setOutputs(0, 0xf0)

            # Bank 1
            lIC7.setInversion(1, 0x00)
            lIC7.setIO(1, 0x00)
            lIC7.setOutputs(1, 0xf0)

            # Tweak the PLL swing
            lI2CBusNode = lDevice.getNode("io.i2c")
            lSIChip = SI534xSlave(lI2CBusNode, lI2CBusNode.getSlave('SI5345').getI2CAddress())

            lSIChip.writeI2CArray(0x113, [0x9, 0x33])


            lDAC1 = DACSlave(lI2CBusNode, lI2CBusNode.getSlave('DAC1').getI2CAddress())
            lDAC2 = DACSlave(lI2CBusNode, lI2CBusNode.getSlave('DAC2').getI2CAddress())

            # BI signals are NIM
            lBISignalThreshold = 0x589D

            lDAC1.setInteralRef(False);
            lDAC2.setInteralRef(False);
            lDAC1.setDAC(7, lBISignalThreshold);
            lDAC2.setDAC(7, lBISignalThreshold);

            secho("DAC1 and DAC2 set to " + hex(lBISignalThreshold), fg='cyan')

        else:
            click.ClickException("Unknown board kind {}".format(lBoardType))

        lIO.getNode('csr.ctrl.rst_lock_mon').write(0x1)
        lIO.getNode('csr.ctrl.rst_lock_mon').write(0x0)
        lIO.getClient().dispatch()

    # Global soft reset
    lIO.getNode('csr.ctrl.soft_rst').write(0x1)
    lDevice.dispatch()

    echo()
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@io.command('freq', short_help="Measure some frequencies.")
@click.pass_obj
def freq(obj):
    lDevice = obj.mDevice
    lBoardType = obj.mBoardType
    lCarrierType = obj.mCarrierType

    secho("PLL Clock frequency measurement:", fg='cyan')
    # Measure the generated clock frequency
    freqs = {}
   
    freqRange=2
    if lBoardType == kBoardTLU:
        freqRange=1
    elif lBoardType == kBoardFIB and lCarrierType == kCarrierAFC:
        freqRange=3

    for i in range(freqRange):
        lDevice.getNode("io.freq.ctrl.chan_sel").write(i)
        lDevice.getNode("io.freq.ctrl.en_crap_mode").write(0)
        lDevice.dispatch()
        time.sleep(2)
        fq = lDevice.getNode("io.freq.freq.count").read()
        fv = lDevice.getNode("io.freq.freq.valid").read()
        lDevice.dispatch()
        freqs[i] = int(fq) * 119.20928 / 1000000 if fv else 'NaN'

    print( "Freq PLL:", freqs[0] )
    if lBoardType != kBoardTLU:
        print( "Freq CDR:", freqs[1] )
        if lBoardType == kBoardFIB and lCarrierType == kCarrierAFC:
            print("Freq BP: ", freqs[2] )
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@io.command('status')
@click.pass_obj
@click.pass_context
@click.option('-v', 'verbose', is_flag=True)
def status(ctx, obj, verbose):
    def decRng( word, ibit, nbits=1):
        return (word >> ibit) & ((1<<nbits)-1)

    lDevice = obj.mDevice
    lBoardType = obj.mBoardType
    lIO = lDevice.getNode('io')

    echo()
    echo( "--- " + style("IO status", fg='cyan') + " ---")
    lCsrStat = toolbox.readSubNodes(lIO.getNode('csr.stat'))
    toolbox.printRegTable(lCsrStat, False)

    if lBoardType == kBoardPC059:
        lActive = [i for i in xrange(8) if ((lCsrStat['sfp_los']>>i) & 0x1) == 0]
        print('Active SFPS:',lActive)
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@io.command('clk-status')
@click.pass_obj
@click.pass_context
@click.option('-v', 'verbose', is_flag=True)
def clkstatus(ctx, obj, verbose):
    def decRng( word, ibit, nbits=1):
        return (word >> ibit) & ((1<<nbits)-1)

    lDevice = obj.mDevice
    lBoardType = obj.mBoardType
    lIO = lDevice.getNode('io')

    echo()
    ctx.invoke(status)

    echo()
    ctx.invoke(freq)
    echo()

    # Access the clock chip
    if lBoardType in [kBoardPC059, kBoardTLU]:
        lI2CBusNode = lIO.getNode("i2c")
        lSIChip = SI534xSlave(lI2CBusNode, lI2CBusNode.getSlave('SI5345').getI2CAddress())
    else:
        lSIChip = lIO.getNode('pll_i2c')

    echo("PLL Configuration id: {}".format(style(lSIChip.readConfigID(), fg='cyan')))
    if verbose:
        secho("PLL Information", fg='cyan')
        lVersion = collections.OrderedDict()
        lVersion['Part number'] = lSIChip.readDeviceVersion()
        lVersion['Device grade'] = lSIChip.readClockRegister(0x4)
        lVersion['Device revision'] = lSIChip.readClockRegister(0x5)
        toolbox.printRegTable(lVersion)

    w = lSIChip.readClockRegister(0xc)

    registers = collections.OrderedDict()
    registers['SYSINCAL'] = decRng(w, 0)
    registers['LOSXAXB'] = decRng(w, 1)
    registers['XAXB_ERR'] = decRng(w, 3)
    registers['SMBUS_TIMEOUT'] = decRng(w, 5)

    w = lSIChip.readClockRegister(0xd)

    registers['LOS'] = decRng(w, 0, 4)
    registers['OOF'] = decRng(w, 4, 4)

    w = lSIChip.readClockRegister(0xe)

    registers['LOL'] = decRng(w, 1)
    registers['HOLD'] = decRng(w, 5)

    w = lSIChip.readClockRegister(0xf)
    registers['CAL_PLL'] = decRng(w, 5)

    w = lSIChip.readClockRegister(0x11)
    registers['SYSINCAL_FLG'] = decRng(w, 0)
    registers['LOSXAXB_FLG'] = decRng(w, 1)
    registers['XAXB_ERR_FLG'] = decRng(w, 3)
    registers['SMBUS_TIMEOUT_FLG'] = decRng(w, 5)

    w = lSIChip.readClockRegister(0x12)
    registers['OOF (sticky)'] = decRng(w, 4, 4)

    secho("PLL Status", fg='cyan')
    toolbox.printRegTable(registers)

# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@io.command('dac-setup')
@click.argument('value', type=click.IntRange(0,0xffff))
@click.pass_obj
@click.pass_context
def dacsetup(ctx, obj, value):

    lDevice = obj.mDevice
    lBoardType = obj.mBoardType
    lI2CBusNode = lDevice.getNode('io.i2c')

    for lSlave in sorted(lI2CBusNode.getSlaves()):
        echo("  {}: {}".format(lSlave, hex(lI2CBusNode.getSlaveAddress(lSlave))))


    lDAC1 = DACSlave(lI2CBusNode, lI2CBusNode.getSlave('DAC1').getI2CAddress())
    lDAC2 = DACSlave(lI2CBusNode, lI2CBusNode.getSlave('DAC2').getI2CAddress())

    # def intRef(dacNode, internal):
    #     if internal:
    #         cmdDAC= [0x00,0x01]
    #     else:
    #         cmdDAC= [0x00,0x00]
    #     dacNode.writeI2CArray(0x38, cmdDAC)

    # def writeDAC(dacNode, chan, dacCode):
    #     print ("DAC value:"  , hex(dacCode))
    #     if chan<0 or chan>7:
    #         print ("writeDAC ERROR: channel",chan,"not in range 0-7 (bit mask)")
    #         return -1
    #     if dacCode<0:
    #         print ("writeDAC ERROR: value",dacCode,"<0. Default to 0")
    #         dacCode=0
    #     elif dacCode>0xFFFF :
    #         print ("writeDAC ERROR: value",dacCode,">0xFFFF. Default to 0xFFFF")
    #         dacCode=0xFFFF

    #     addr = 0x18 + ( chan & 0x7)
    #     seq = [
    #         ((dacCode >> 8) & 0xff),
    #         (dacCode & 0xff),
    #     ]

    #     dacNode.writeI2CArray(addr, seq)

    # intRef(lDAC1, False)
    # intRef(lDAC2, False)

    # writeDAC(lDAC1, 7, 0x589D )
    # writeDAC(lDAC2, 7, 0x589D )
    # dacval = 0x589D

    lDAC1.setInteralRef(False);
    lDAC2.setInteralRef(False);
    lDAC1.setDAC(7, value);
    lDAC2.setDAC(7, value);

    secho("DAC1 and DAC2 set to " + hex(value), fg='cyan')


# ------------------------------------------------------------------------------

#sfp related functions
def GetCalibSlopeFromSFP(lSFP,index):
    # calib slope addresses, each slope is 16 bits
    slope_adr = [ [0x4C, 0x4D], #laser current
                  [0x50, 0x51], #tx_pwr
                  [0x54, 0x55], #temp
                  [0x58, 0x59] ]#voltage supply
    slope_whole = lSFP.readI2C(0x51,slope_adr[index][0])
    slope_decimal = lSFP.readI2C(0x51,slope_adr[index][1]) / 256.0
    return slope_whole+slope_decimal

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
#####

# temperature
def GetSFPTemperatureRaw(lSFP):
    temp_raw_whole = lSFP.readI2C(0x51,0x60)
    #first bit corresponds to sign
    if (temp_raw_whole & (1 << (8 - 1))) != 0:
        temp_raw_whole = temp_raw_whole - (1 << 8)
    temp_raw_decimal = lSFP.readI2C(0x51,0x61) / 256.0
    return temp_raw_whole + temp_raw_decimal

def GetSFPTemperatureCalibrated(lSFP):
    temp_raw = GetSFPTemperatureRaw(lSFP)
    slope = GetCalibSlopeFromSFP(lSFP,2)
    offset = GetCalibOffsetFromSFP(lSFP,2)
    return (temp_raw*slope) + offset
#######

# voltage
def GetSFPVoltageRaw(lSFP):
    return (lSFP.readI2C(0x51,0x62) << 8) | lSFP.readI2C(0x51,0x63)

def GetSFPVoltageCalibrated(lSFP):
    voltage_raw = GetSFPVoltageRaw(lSFP)
    slope = GetCalibSlopeFromSFP(lSFP,3)
    offset = GetCalibOffsetFromSFP(lSFP,3)
    return ((voltage_raw*slope)+offset)*1e-4
#########


# rx power
def GetSFPRxPowerRaw(lSFP):
    return (lSFP.readI2C(0x51,0x68) << 8) | lSFP.readI2C(0x51,0x69)

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
        rx_pars.append(par)

    rx_pars_counter=0
    rx_pwr_calib=0
    for par in rx_pars:
        rx_pwr_calib = rx_pwr_calib + (par*pow(rx_pwr_raw,rx_pars_counter))
        rx_pars_counter=rx_pars_counter+1
    return rx_pwr_calib*0.1
#####

# tx power
def GetSFPTxPowerRaw(lSFP):
    return (lSFP.readI2C(0x51,0x66) << 8) | lSFP.readI2C(0x51,0x67)

def GetSFPTxPowerCalibrated(lSFP):
    tx_power_raw = GetSFPTxPowerRaw(lSFP)
    slope = GetCalibSlopeFromSFP(lSFP,1)
    offset = GetCalibOffsetFromSFP(lSFP,1)
    return ((tx_power_raw*slope) + offset)*0.1
######


# current
def GetSFPCurrentRaw(lSFP):
    return (lSFP.readI2C(0x51,0x64) << 8) | lSFP.readI2C(0x51,0x65)

def GetSFPCurrentCalibrated(lSFP):
    current_raw = GetSFPCurrentRaw(lSFP)
    slope = GetCalibSlopeFromSFP(lSFP,0)
    offset = GetCalibOffsetFromSFP(lSFP,0)
    return ((current_raw*slope) + offset)*0.002
####

# ------------------------------------------------------------------------------
@io.command('sfp-status', short_help="Read SFP parameters.")
@click.pass_obj
@click.pass_context
def sfpstatus(ctx, obj):
    '''
    Read SFP status
    '''

    lDevice = obj.mDevice
    lBoardType = obj.mBoardType
    lCarrierType = obj.mCarrierType
    lDesignType = obj.mDesignType

    lIO = lDevice.getNode('io')

    if ( lBoardType != kBoardFMC ):
        click.ClickException("Only timing FMC supported at the moment")

    lSFP = lIO.getNode('sfp_i2c')

    # vendor name
    vendor=''
    for adr in range(0x14, 0x23):
        char = lSFP.readI2C(0x50,adr)
        vendor=vendor+chr(char)

    #vendor part number
    pn=''
    for adr in range(0x28, 0x37):
        char = lSFP.readI2C(0x50,adr)
        pn=pn+chr(char)

    echo("SFP Vendor '{}' PN '{}' \n".format(
        style(vendor, fg='blue'),
        style(pn, fg='blue')
    ))

    #mon_diag_type = lSFP.readI2C(0x50,0x5C)
    #print("Monitor diag type: "+format(mon_diag_type, '08b'))

    secho("Measured SFP parameters", fg='cyan')
    temp_calib = GetSFPTemperatureCalibrated(lSFP)
    print("Temperature        : {:.1f} C".format(temp_calib))
    
    voltage_calib = GetSFPVoltageCalibrated(lSFP)
    print("Supply voltage     : {:.1f} V".format(voltage_calib))

    rx_power_calib=GetSFPRxPowerCalibrated(lSFP)
    print("Rx power           : {:.1f} uW".format(rx_power_calib))

    tx_power_calib = GetSFPTxPowerCalibrated(lSFP)
    print("Tx power           : {:.1f} uW".format(tx_power_calib))

    current_calib = GetSFPCurrentCalibrated(lSFP)
    print("Laser bias current : {:.1f} mA".format(current_calib))

    echo("")

    secho("Tx disable controls", fg='cyan')

    #check if sfp supports sft tx control
    enhanced_options = lSFP.readI2C(0x50,0x5d)

    # bit 6 tells us whether the soft tx control is implemented in this sfp
    soft_tx_control_enabled_mask = 0b01000000
    soft_tx_control_enabled = enhanced_options&soft_tx_control_enabled_mask

    print("Tx disable reg supported: {}".format(soft_tx_control_enabled != 0))

    # get optional status/control bits
    opt_status_ctrl_byte = lSFP.readI2C(0x51,0x6e)    

    # bit 7 tells us the state of tx_disble pin
    tx_diable_pin_state = 1 if opt_status_ctrl_byte & (1 << 7) != 0 else 0
    print("Tx disable pin     : {}".format(tx_diable_pin_state))

    # bit 6 tells us the state of tx_disble register
    tx_diable_reg_state = 1 if opt_status_ctrl_byte & (1 << 6) != 0 else 0
    print("Tx disable reg     : {}".format(tx_diable_reg_state))
#    echo("")
#    
#    print("Rx power low alarm thresh  : {}".format( ((lSFP.readI2C(0x51,0x22) << 8) | lSFP.readI2C(0x51,0x23))*0.1 ))
#    print("Rx power low warning thresh: {}".format( ((lSFP.readI2C(0x51,0x26) << 8) | lSFP.readI2C(0x51,0x27))*0.1 ))
#
#
#    print("Rx power high warning thresh: {}".format( ((lSFP.readI2C(0x51,0x24) << 8) | lSFP.readI2C(0x51,0x25))*0.1 ))
#    print("Rx power high alarm thresh  : {}".format( ((lSFP.readI2C(0x51,0x20) << 8) | lSFP.readI2C(0x51,0x21))*0.1 ))
#
#    echo("")
#
#    print("Tx power low alarm thresh  : {}".format( ((lSFP.readI2C(0x51,0x1A) << 8) | lSFP.readI2C(0x51,0x1B))*0.1 ))
#    print("Tx power low warning thresh: {}".format( ((lSFP.readI2C(0x51,0x1E) << 8) | lSFP.readI2C(0x51,0x1F))*0.1 ))
#
#    print("Tx power high warning thresh: {}".format( ((lSFP.readI2C(0x51,0x1C) << 8) | lSFP.readI2C(0x51,0x1D))*0.1 ))
#    print("Tx power high alarm thresh  : {}".format( ((lSFP.readI2C(0x51,0x18) << 8) | lSFP.readI2C(0x51,0x19))*0.1 ))
    #####
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@io.command('switch-sfp-tx', short_help="Control sfp tx")
@click.option('--on/--off', default=False, help='enable/disable tx; default: FALSE')
@click.pass_obj
def switchsfptx(obj, on):

    lDevice = obj.mDevice
    lBoardType = obj.mBoardType
    lCarrierType = obj.mCarrierType
    lDesignType = obj.mDesignType

    lIO = lDevice.getNode('io')

    if ( lBoardType != kBoardFMC ):
        raise click.ClickException("Only timing FMC supported at the moment")

    lSFP = lIO.getNode('sfp_i2c')

    # vendor name
    vendor=''
    for adr in range(0x14, 0x23):
        char = lSFP.readI2C(0x50,adr)
        vendor=vendor+chr(char)

    #vendor part number
    pn=''
    for adr in range(0x28, 0x37):
        char = lSFP.readI2C(0x50,adr)
        pn=pn+chr(char)

    echo("SFP Vendor '{}' PN '{}' \n".format(
        style(vendor, fg='blue'),
        style(pn, fg='blue')
    ))

    #check if sfp supports sft tx control
    enhanced_options = lSFP.readI2C(0x50,0x5d)
    #print(format(enhanced_options, '08b'))

    # bit 6 tells us whether the soft tx control is implemented in this sfp
    soft_tx_control_enabled_mask = 0b01000000

    soft_tx_control_enabled = enhanced_options&soft_tx_control_enabled_mask
    #print(format(soft_tx_control_enabled, '08b'))
    #print(soft_tx_control_enabled)

    if (not soft_tx_control_enabled):
        secho("WARNING Soft tx disable not supported by this SFP \n", fg='red')

    # get optional status/control bits
    opt_status_ctrl_byte = lSFP.readI2C(0x51,0x6e)    
    secho("Tx parameters", fg='cyan')

    # bit 7 tells us the state of tx_disble pin
    tx_diable_pin_state = 1 if opt_status_ctrl_byte & (1 << 7) != 0 else 0
    print("Tx disable pin     : {}".format(tx_diable_pin_state))

    # bit 6 tells us the state of tx_disble register
    tx_diable_reg_state = 1 if opt_status_ctrl_byte & (1 << 6) != 0 else 0
    print("Tx disable reg     : {}".format(tx_diable_reg_state))

    tx_power_calib = GetSFPTxPowerCalibrated(lSFP)
    print("Tx power           : {:.1f} uW".format(tx_power_calib))

    current_calib = GetSFPCurrentCalibrated(lSFP)
    print("Laser bias current : {:.1f} mA".format(current_calib))

    echo ("")

    #bit 6 also controls the soft the tx
    if (on):
        new_opt_status_ctrl_byte = opt_status_ctrl_byte & ~(1 << 6)        
        secho("Setting tx_disble reg to 0 (switch on)")    
    else:
        new_opt_status_ctrl_byte = opt_status_ctrl_byte | 1 << 6;
        secho("Setting tx_disble reg to 1 (switch off)")    
    lSFP.writeI2C(0x51,0x6e,new_opt_status_ctrl_byte)
    
    time.sleep(0.2)

    echo("")

    # get optional status/control bits
    opt_status_ctrl_byte = lSFP.readI2C(0x51,0x6e)
    
    secho("Tx parameters", fg='cyan')

    # bit 7 tells us the state of tx_disble pin
    tx_diable_pin_state = 1 if opt_status_ctrl_byte & (1 << 7) != 0 else 0
    print("Tx disable pin     : {}".format(tx_diable_pin_state))

    # bit 6 tells us the state of tx_disble register
    tx_diable_reg_state = 1 if opt_status_ctrl_byte & (1 << 6) != 0 else 0
    print("Tx disable reg     : {}".format(tx_diable_reg_state))

    tx_power_calib = GetSFPTxPowerCalibrated(lSFP)
    print("Tx power           : {:.1f} uW".format(tx_power_calib))

    current_calib = GetSFPCurrentCalibrated(lSFP)
    print("Laser bias current : {:.1f} mA".format(current_calib))
# ------------------------------------------------------------------------------