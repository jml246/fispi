#!/bin/bash
#cd ~/fispi
#echo "starting fiscore"
#~/fispi/fiscore/fiscore &
#echo "starting fisalg"
#~/fispi/fisalg/fisalg &
#echo "starting fisio"
#sudo -u pi ~/fispi/fisio/fisio &
#echo "end"
  if (( $(ps aux | grep -c "sudo ./fiscore") >= 3 ));
  then
      echo "fiscore still active"
  else
      echo "Process fiscore not running in screen session!"
#      #-S names the process under screen
      echo "Restarting screen  and Sending fiscore command to screen session"
      sudo -u pi screen -S fiscore -X quit
      sudo -u pi screen -S fisalg -X quit
      sudo -u pi screen -S fisio -X quit
      echo "...done"
      echo "Restarting fiscore process"
       sudo -u pi screen -S fiscore -dm bash -c 'cd /home/pi/fispi/fiscore/;sudo ./fiscore; exec bash'
       #cd /home/pi/fispi/fiscore/; sudo ./fiscore > /home/pi/fiscorelog 2>&1
      echo "Restarting fisalg process"
       sudo -u pi screen -S fisalg -dm bash -c 'cd /home/pi/fispi/fisalg/;sudo ./fisalg; exec bash'
       #cd /home/pi/fispi/fisalg/; sudo ./fisalg > /home/pi/fisalglog 2>&1
      echo "Restarting fisio process"
       sudo -u pi screen -S fisio -dm bash -c 'cd /home/pi/fispi/fisio/;sudo ./fisio; exec bash'
       #cd /home/pi/fispi/fisio/; sudo ./fisio > /home/pi/fisiolog 2>&1
      echo "...done"
  fi

