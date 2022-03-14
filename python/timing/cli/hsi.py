import click
import sys

import collections

from . import toolbox
import timing.common.definitions as defs
from timing.common.definitions import kLibrarySupportedBoards, kLibrarySupportedDesigns

from click import echo, style, secho
from .click_texttable import Texttable
import time

# ------------------------------------------------------------------------------
#    ____        __          _      __ 
#   / __/__  ___/ /__  ___  (_)__  / /_
#  / _// _ \/ _  / _ \/ _ \/ / _ \/ __/
# /___/_//_/\_,_/ .__/\___/_/_//_/\__/ 
#              /_/                     
@click.group('hsi', invoke_without_command=True)
@click.pass_obj
@click.argument('device', callback=toolbox.validate_device, autocompletion=toolbox.completeDevices)
def hsi(obj, device):
    '''
    HSI commands.

    \b
    DEVICE: uhal device identifier
    IDS: id(s) of the target endpoint(s).
    '''

    lDevice = obj.mConnectionManager.getDevice(str(device))
    if obj.mTimeout:
        lDevice.setTimeoutPeriod(obj.mTimeout)

    
    echo('Created HSI device')
    lTopDesign = lDevice.getNode('')
    lBoardInfo = toolbox.readSubNodes(lDevice.getNode('io.config'), False)
    lDevice.dispatch()

    if lBoardInfo['board_type'].value() in kLibrarySupportedBoards and lBoardInfo['design_type'].value() in kLibrarySupportedDesigns:
        lTopDesign.validate_firmware_version()
        try:
            echo(lDevice.getNode('io').get_hardware_info())
        except:
            secho("Failed to retrieve hardware information I2C issue? Initial board reset needed?", fg='yellow')
            e = sys.exc_info()[0]
            secho("Error: {}".format(e), fg='red')

    obj.mDevice = lDevice
    obj.mHSIEndpoint = lDevice.getNode('endpoint0')
    obj.mTopDesign = lDevice.getNode('')
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@hsi.command('status')
@click.pass_obj
@click.pass_context
def status(ctx, obj):
    '''
    Print the status of CRT endpoint wrapper block.
    '''

    lDevice = obj.mDevice
    lHSIEpt = obj.mHSIEndpoint
    echo(lHSIEpt.get_status())
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@hsi.command('enable', short_help="Configure the HSI endpoint for running")
@click.pass_obj
@click.pass_context
@click.argument('action', default='on', type=click.Choice(['on', 'off', 'reset']))
@click.option('--partition', '-p', type=click.IntRange(0,4), help='Partition', default=0)
@click.option('--address', '-a', type=toolbox.IntRange(0x0,0x100), help='Address', default=0)
def enable(ctx, obj, action, partition, address):
    '''
    Activate the timing endpoint in the hsi wrapper block.
    '''

    lDevice = obj.mDevice
    lHSIEpt = obj.mHSIEndpoint
    
    if action == 'off':
        lHSIEpt.disable()
    elif action == 'on':
        lHSIEpt.enable(address=address,partition=partition)
    elif action == 'reset':
        lHSIEpt.reset(address=address,partition=partition)
        lHSIEpt.reset_hsi()

    time.sleep(0.1)
    ctx.invoke(status)
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@hsi.command('configure', short_help="Configure the HSI block for running")
@click.pass_obj
@click.pass_context
@click.option('--src', '-s', type=click.IntRange(0x0,0x1), help='Source of HSI data,; 0: hardware; 1: timestamp (emulation mode)', default=0)
@click.option('--re-mask', '-r', type=click.IntRange(0,0xffffffff), help='Rising edge mask', default=0)
@click.option('--fe-mask', '-f', type=click.IntRange(0,0xffffffff), help='Falling edge mask', default=0)
@click.option('--inv-mask', '-i', type=click.IntRange(0,0xffffffff), help='Invert mask', default=0)
@click.option('--rate', type=float, help='Random trigger rate [Hz] on bit 0 in emulation mode', default=1)
def configure(ctx, obj, src, re_mask, fe_mask, inv_mask, rate):
    '''
    Configure the hsi in the hsi wrapper block.
    '''

    lDevice = obj.mDevice
    lHSIEpt = obj.mHSIEndpoint
    lTopDesign = obj.mTopDesign

    lHSIEpt.reset_hsi()
    lTopDesign.configure_hsi(src, re_mask, fe_mask, inv_mask, rate)
    lHSIEpt.start_hsi()
    secho("HSI configured (and started)", fg='green')

    time.sleep(0.1)
    ctx.invoke(status)
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@hsi.command('start', short_help='Start the hsi triggering and the writing events into buffer.')
@click.pass_obj
@click.pass_context
def readback(ctx, obj):
    '''
    Read the content of the endpoint master readout buffer.
    '''
    lDevice = obj.mDevice
    lHSIEpt = obj.mHSIEndpoint

    lHSIEpt.start_hsi()
    secho("HSI start", fg='green')

    time.sleep(0.1)
    ctx.invoke(status)
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@hsi.command('stop', short_help='Stop the hsi triggering and the writing events into buffer.')
@click.pass_obj
@click.pass_context
def readback(ctx, obj):
    '''
    Read the content of the endpoint master readout buffer.
    '''
    lDevice = obj.mDevice
    lHSIEpt = obj.mHSIEndpoint

    lHSIEpt.stop_hsi()
    secho("HSI stop", fg='green')

    time.sleep(0.1)
    ctx.invoke(status)
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@hsi.command('readback', short_help='Read the content of the hsi readout buffer.')
@click.pass_obj
@click.pass_context
@click.option('--all/--events', '-a/ ', 'readall', default=False, help="Buffer readout mode.\n- events: only completed events are readout.\n- all: the content of the buffer is fully read-out.")
def readback(ctx, obj, readall):
    '''
    Read the content of the endpoint master readout buffer.
    '''
    lDevice = obj.mDevice
    lHSIEpt = obj.mHSIEndpoint
    
    echo(lHSIEpt.get_data_buffer_table(readall,False))
# ------------------------------------------------------------------------------