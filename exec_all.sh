codes=`find examples/*.ho`

for src in $codes; do
  echo "$src:"
  build/ho $src
  echo
done
