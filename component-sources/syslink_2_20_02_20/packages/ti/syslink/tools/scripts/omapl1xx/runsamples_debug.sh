#!/bin/sh
#   
#   @file   runsamples_debug.sh
#
#   @brief  Script to run the samples
#
#
#   ============================================================================
#
#   Copyright (c) 2008-2012, Texas Instruments Incorporated
#
#   Redistribution and use in source and binary forms, with or without
#   modification, are permitted provided that the following conditions
#   are met:
#   
#   *  Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#   
#   *  Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
#   
#   *  Neither the name of Texas Instruments Incorporated nor the names of
#      its contributors may be used to endorse or promote products derived
#      from this software without specific prior written permission.
#   
#   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#   PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
#   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
#   OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
#   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
#   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
#   EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#   Contact information for paper mail:
#   Texas Instruments
#   Post Office Box 655303
#   Dallas, Texas 75265
#   Contact information: 
#   http://www-k.ext.ti.com/sc/technical-support/product-information-centers.htm?
#   DCMP=TIHomeTracking&HQS=Other+OT+home_d_contact
#   ============================================================================
#   
DIR="$( cd "$(dirname "$0")" && pwd)"

echo "======== Running messageq app ========"
cd $DIR/messageq
./run_messageqapp_debug.sh

echo "======== Running listmp app ========"
cd $DIR/listmp
./run_listmpapp_debug.sh

echo "======== Running heampbufmp app ========"
cd $DIR/heapbufmp
./run_heapbufmpapp_debug.sh

echo "======== Running heapmemmp app ========"
cd $DIR/heapmemmp
./run_heapmemmpapp_debug.sh

echo "======== Running gatemp app ========"
cd $DIR/gatemp
./run_gatempapp_debug.sh

echo "======== Running notify app ========"
cd $DIR/notify
./run_notifyapp_debug.sh

echo "======== Running frameq app ========"
cd $DIR/frameq
./run_frameqapp_debug.sh

echo "======== Running ringio app ========"
cd $DIR/ringio
./run_ringio_debug.sh

# Sample not install(ed) since Ipc_stop not implemented on RTOS-side
#echo "======== Running ringiogpp app ========"
#cd $DIR/ringiogpp
#./run_ringiogpp_debug.sh

# Sample not install(ed) since Ipc_stop not implemented on RTOS-side
#echo "======== Running sharedregion app ========"
#cd $DIR/sharedregion
#./run_sharedregionapp_debug.sh
