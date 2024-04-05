#!/bin/bash

COUNTER_SUCCESS=0
COUNTER_FAIL=0
DIFF_RES=""
TEST_FILE="test1.txt test2.txt"
FANDE="-f reg.txt -e 123"
#FANDE="-f reg_no_match.txt -e 777"
echo "" > log.txt
if [[ ! -s ./s21_grep ]]; then
    echo  "there's no s21_grep executable file"
    exit
fi

# 1 flag
for var in -i -v -c -l -n -h -s -o
do
          TEST1="$var $FANDE $TEST_FILE"
          echo "$TEST1"
          ./s21_grep $TEST1 > s21_grep.txt
          grep $TEST1 > grep.txt
          DIFF_RES="$(diff -s s21_grep.txt grep.txt)"
          if [ "$DIFF_RES" == "Files s21_grep.txt and grep.txt are identical" ]
            then
              (( COUNTER_SUCCESS++ ))
            else
              echo "$TEST1" >> log.txt
              (( COUNTER_FAIL++ ))
          fi
          rm s21_grep.txt grep.txt
done

# 2 flags
for var in -i -v -c -l -n -h -s -o
do
  for var2 in -i -v -c -l -n -h -s -o
  do
        if [ $var != $var2 ]
        then
          TEST1="$var $var2 $FANDE $TEST_FILE"
          echo "$TEST1"
          ./s21_grep $TEST1 > s21_grep.txt
          grep $TEST1 > grep.txt
          DIFF_RES="$(diff -s s21_grep.txt grep.txt)"
          if [ "$DIFF_RES" == "Files s21_grep.txt and grep.txt are identical" ]
            then
              (( COUNTER_SUCCESS++ ))
            else
              echo "$TEST1" >> log.txt
              (( COUNTER_FAIL++ ))
          fi
          rm s21_grep.txt grep.txt
        fi
  done
done

# 3 flags
for var in -i -v -c -l -n -h -s -o
do
  for var2 in -i -v -c -l -n -h -s -o
  do
      for var3 in -i -v -c -l -n -h -s -o
      do
        if [ $var != $var2 ] && [ $var2 != $var3 ] && [ $var != $var3 ]
        then
          TEST1="$var $var2 $var3 $FANDE $TEST_FILE"
          echo "$TEST1"
          ./s21_grep $TEST1 > s21_grep.txt
          grep $TEST1 > grep.txt
          DIFF_RES="$(diff -s s21_grep.txt grep.txt)"
          if [ "$DIFF_RES" == "Files s21_grep.txt and grep.txt are identical" ]
            then
              (( COUNTER_SUCCESS++ ))
            else
              echo "$TEST1" >> log.txt
              (( COUNTER_FAIL++ ))
          fi
          rm s21_grep.txt grep.txt

        fi
      done
  done
done

# 4 flags
# for var in -i -v -c -l -n -h -s -o
# do
#   for var2 in -i -v -c -l -n -h -s -o
#   do
#       for var3 in -i -v -c -l -n -h -s -o
#       do
#           for var4 in -i -v -c -l -n -h -s -o
#           do
#             if [ $var != $var2 ] && [ $var2 != $var3 ] && [ $var != $var3 ] && [ $var != $var4 ] && [ $var2 != $var4 ] && [ $var3 != $var4 ]
#             then
#               TEST1="$var $var2 $var3 $var4 $F_AND_E $TEST_FILE"
#               echo "$TEST1"
#               ./s21_grep $TEST1 > s21_grep.txt
#               grep $TEST1 > grep.txt
#               DIFF_RES="$(diff -s s21_grep.txt grep.txt)"
#               if [ "$DIFF_RES" == "Files s21_grep.txt and grep.txt are identical" ]
#                 then
#                   (( COUNTER_SUCCESS++ ))
#                 else
#                   echo "$TEST1" >> log.txt
#                   (( COUNTER_FAIL++ ))
#               fi
#               rm s21_grep.txt grep.txt
#             fi
#           done
#       done
#   done
# done

echo "SUCCESS: $COUNTER_SUCCESS"
echo "FAIL: $COUNTER_FAIL"