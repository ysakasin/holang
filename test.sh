codes="fib.ho test.ho sample.ho method.ho"

pass=0
fail=0
for src in $codes; do
  printf "$src: "
  ./ho $src 1> /dev/null
  if [ $? = 0 ]; then
    printf "\e[32m"
    echo "PASS"
    pass=`expr $pass + 1`
  else 
    printf "\e[31m"
    echo "$src: FAIL"
    fail=`expr $fail + 1`
  fi
  printf "\e[m"
done

echo
printf "\e[32m$pass passed\e[m, \e[31m$fail fails\e[m\n"
