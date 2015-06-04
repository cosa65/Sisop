nro=$(head -c 1 nrodecom.txt)
nro=$(($nro+1))
echo $nro elefantes se balanceaban sobre la tela de una araña>nrodecom.txt
#sed 's/$nro/$nronuevo/' <nrodecom.txt >nrodecom1.txt
git add --all && git commit -m>(nrodecom.txt) && git push