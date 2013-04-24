scp -P 13022 -r ../mantouDB/ mapred@192.168.1.163:/home/mapred/mantouDB
scp -P 13022 -r ../mantouDB/ mapred@192.168.1.183:/home/mapred/mantouDB
scp -P 13022 -r ../mantouDB/ mapred@192.168.1.184:/home/mapred/mantouDB
ssh -p 13022  mapred@192.168.1.163 cd /home/mapred/mantouDB/mantouDB make
ssh -p 13022  mapred@192.168.1.183 cd /home/mapred/mantouDB/mantouDB make
ssh -p 13022  mapred@192.168.1.184 cd /home/mapred/mantouDB/mantouDB make

