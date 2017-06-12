sed  \
	-e 's/|[^|]*(PT)//g' -e 's/\t[^\t]*(PT)/\t/g' \
	-e 's/|[^|]*(CN)//g' -e 's/\t[^\t]*(CN)/\t/g' \
	-e 's/|[^|]*(AB)//g' -e 's/\t[^\t]*(AB)/\t/g' \
	-e 's/|[^|]*(SY)//g' -e 's/\t[^\t]*(SY)/\t/g' \
	-e 's/|[^|]*(SN)//g' -e 's/\t[^\t]*(SN)/\t/g' \
	-e 's/|[^|]*(AQS)//g' -e 's/\t[^\t]*(AQS)/\t/g' \
	-e 's/|[^|]*(DN)//g' -e 's/\t[^\t]*(DN)/\t/g' \
	-e 's/|[^|]*(HD)//g' -e 's/\t[^\t]*(HD)/\t/g' \
	< $1

	
