import click
import sys

import collections

from . import toolbox
import timing.common.definitions as defs
from timing.common.definitions import kLibrarySupportedBoards, kLibrarySupportedDesigns
from timing.common.toolbox import format_firmware_version

from click import echo, style, secho
from .click_texttable import Texttable
import time

# ------------------------------------------------------------------------------
#    ____        __          _      __ 
#   / __/__  ___/ /__  ___  (_)__  / /_
#  / _// _ \/ _  / _ \/ _ \/ / _ \/ __/
# /___/_//_/\_,_/ .__/\___/_/_//_/\__/ 
#              /_/                     
@click.group('ept', invoke_without_command=True)
@click.pass_obj
@click.argument('device', callback=toolbox.validate_device, autocompletion=toolbox.completeDevices)
@click.argument('ids', callback=toolbox.split_ints)
def endpoint(obj, device, ids):
    '''
    Endpoint master commands.

    \b
    DEVICE: uhal device identifier
    IDS: id(s) of the target endpoint(s).
    '''

    lDevice = obj.mConnectionManager.getDevice(str(device))
    if obj.mTimeout:
        lDevice.setTimeoutPeriod(obj.mTimeout)

    echo('Created endpoint device ' + style(lDevice.id(), fg='blue'))
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
    
    # Ensure that target endpoint exists
    lEPNames = lDevice.getNodes('endpoint('+'|'.join( ( str(i) for i in ids ) )+')')

    if len(lEPNames) != len(ids):
        lEPNotFound = set( ( 'endpoint{}'.format(i) for i in ids ) ) -set(lEPNames)
        raise click.ClickException('Endpoints {} do(es) not exist'.format(', '.join( ('\''+ep+'\'' for ep in lEPNotFound) )))
    
    lEndpoints = { pid:lDevice.getNode('endpoint{}'.format(pid)) for pid in ids}

    lVersions = { pid:n.read_version() for pid,n in lEndpoints.items()}

    if len(set( (v for v in lVersions.values()) )) > 1:
        secho('WARNING: multiple enpoint versions detected', fg='yellow')
        secho()

    lVTable = Texttable(max_width=0)
    lVTable.set_deco(Texttable.VLINES | Texttable.BORDER)
    lVTable.set_chars(['-', '|', '+', '-'])
    lVTable.header( sorted(lVersions.keys()) )
    lVTable.add_row( [format_firmware_version(lVersions[p]) for p in sorted(lVersions.keys()) ] )
    echo  ( lVTable.draw() )

    obj.mDevice = lDevice
    obj.mEndpoints = lEndpoints
    obj.mIO = lDevice.getNode('io')
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@endpoint.command('freq', short_help="Measure some frequencies.")
@click.pass_obj
def freq(obj):

    for i, ep in obj.mEndpoints.items():

        secho("Endpoint frequency measurement:", fg='cyan')
        # Measure the generated clock frequency
        freq = ep.read_clock_frequency()

        echo( "Endpoint {} freq MHz : {}".format(i, freq) )
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@endpoint.command('enable')
@click.argument('action', default='on', type=click.Choice(['on', 'off', 'reset']))
@click.option('--partition', '-p', type=click.IntRange(0,4), help='Partition', default=0)
@click.option('--address', '-a', type=toolbox.IntRange(0x0,0x100), help='Address', default=0)
@click.pass_obj
@click.pass_context
def enable(ctx, obj, action, partition, address):
    '''
    Activate timing endpoint wrapper block.
    '''

    lDone = []
    for i, ep in obj.mEndpoints.items():
        if action == 'off':
            ep.disable()
        elif action == 'on':
            ep.enable(address=address,partition=partition)
        elif action == 'reset':
            ep.reset(address=address,partition=partition)
        lDone.append(i)

    time.sleep(0.1)
    ctx.invoke(status)

    echo(
        "> Endpoints {} ".format(','.join( (str(i) for i in lDone) )) + style(action+(" in partition {}".format(partition) if action in ['on', 'reset'] else ""), fg='blue'))
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@endpoint.command('status', short_help='Display the status of timing endpoint.')
@click.pass_obj
@click.option('--watch', '-w', is_flag=True, default=False, help='Turn on automatic refresh')
@click.option('--period', '-p', type=click.IntRange(0, 240), default=2, help='Period of automatic refresh')
def status(obj, watch, period):
    '''
    Display the endpoint status, accepted and rejected command counters
    '''
    lNumCtrs = 0x10

    lDevice = obj.mDevice
    lEndpoints = obj.mEndpoints
    lEndPointNode = lDevice.getNode('endpoint0')
    lIO = obj.mIO

    firmware_clock_frequency_hz = lIO.read_firmware_frequency()

    while(True):
        if watch:
            click.clear()
        
        lEPKeys = sorted(lEndpoints)
        lEPData = { p:{} for p,_ in lEndpoints.items() }

        for p,n in lEndpoints.items():

            lData = lEPData[p]
            lData['tstamp'] = n.getNode('tstamp').readBlock(2)
            lData['evtctr'] = n.getNode('evtctr').read()
            lData['bufcount'] = n.getNode('buf.count').read()

            lData['ctrldump'] = toolbox.readSubNodes(n.getNode('csr.ctrl'), False)
            lData['statdump'] = toolbox.readSubNodes(n.getNode('csr.stat'), False)
            lData['ctrs'] = n.getNode('ctrs').readBlock(lNumCtrs)
        lDevice.dispatch()


        lTimeStamp = lEndPointNode.read_timestamp()

        lEPSummary = Texttable(max_width=0)
        lEPSummary.set_deco(Texttable.VLINES | Texttable.BORDER | Texttable.HEADER )
        lEPSummary.set_chars(['-', '|', '+', '-'])
        lEPSummary.header( ['Endpoint']+lEPKeys )
        lEPSummary.set_cols_dtype(['t']*(len(lEPKeys)+1))
        lEPSummary.add_row(
                ['State']+
                [toolbox.fmtEpState(lEPData[p]['statdump']['ep_stat']) for p in lEPKeys
                ]
        )
        lEPSummary.add_row(
                ['Partition']+
                [str(lEPData[p]['ctrldump']['tgrp']) for p in lEPKeys]
        )
        lEPSummary.add_row(
                ['Address']+
                [str(lEPData[p]['ctrldump']['addr']) for p in lEPKeys]
        )
        lEPSummary.add_row(
                ['Timestamp']+
                [style(str(toolbox.formatTStamp(lEPData[p]['tstamp'],firmware_clock_frequency_hz)), fg='blue') for p in lEPKeys]
        )
        lEPSummary.add_row(
                ['Timestamp (hex)']+
                [hex(toolbox.tstamp2int(lEPData[p]['tstamp'])) for p in lEPKeys]
        )        
        lEPSummary.add_row(
                ['EventCounter']+
                [str(lEPData[p]['evtctr']) for p in lEPKeys]
        )
        lEPSummary.add_row(
                ['Buffer status']+
                [style('OK', fg='green') if (lEPData[p]['statdump']['buf_err'] == 0x0) else style('Error', fg='red') for p in lEPKeys]
        )
        lEPSummary.add_row(
                ['Buffer occupancy']+
                [str(lEPData[p]['bufcount']) for p in lEPKeys]
        )
        echo ( lEPSummary.draw() )

        echo()
        echo( "--- " + style("Endpoint state", fg='cyan') + " ---")

        lEPStats = Texttable(max_width=0)
        lEPStats.set_deco(Texttable.VLINES | Texttable.BORDER | Texttable.HEADER)
        lEPStats.set_chars(['-', '|', '+', '-'])
        lEPStats.set_cols_align(['l']+['c']*len(lEPKeys))
        lEPStats.set_cols_width([10]+[8]*(len(lEPKeys)))

        lEPStats.header( ['Endpoint']+lEPKeys )

        for k in sorted(lEPData[lEPKeys[0]]['statdump']):
            lEPStats.add_row(
                [k]+
                [
                    style(hex(v), fg=('blue' if v != 0 else 'white')) 
                    for v in ( lEPData[p]['statdump'][k]  for p in lEPKeys ) 
                ]
            )
        echo ( lEPStats.draw() )

        echo()
        echo( "--- " + style("Command counters", fg='cyan') + " ---")

        lEPCtrs = Texttable(max_width=0)
        lEPCtrs.set_deco(Texttable.VLINES | Texttable.BORDER | Texttable.HEADER)
        lEPCtrs.set_chars(['-', '|', '+', '-'])
        lEPCtrs.set_cols_align(['l']+['c']*len(lEPKeys))
        lEPCtrs.set_cols_width([12]+[8]*(len(lEPKeys)))

        lEPCtrs.header( ['Endpoint']+lEPKeys )
        for c in range(lNumCtrs):
            lEPCtrs.add_row(
                [defs.kCommandNames.get(c)]+
                [k if k > 0 else '' for k in (lEPData[p]['ctrs'][c] for p in lEPKeys)]
                )
        echo ( lEPCtrs.draw() )

        if watch:
            time.sleep(period)
        else:
            break
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@endpoint.command('readback', short_help='Read the content of the endpoint master readout buffer.')
@click.pass_obj
@click.option('--all/--events', '-a/ ', 'readall', default=False, help="Buffer readout mode.\n- events: only completed events are readout.\n- all: the content of the buffer is fully read-out.")
def readback(obj, readall):
    '''
    Read the content of the endpoint master readout buffer.
    '''
    lDevice = obj.mDevice
    lEndpoints = obj.mEndpoints
    
    for p,n in lEndpoints.items():

        echo(n.get_data_buffer_table(readall))
# ------------------------------------------------------------------------------