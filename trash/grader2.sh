set srcdir=~csci570b/public/cs402/warmup1

#
# If using C (well, you have to use C):
#
        /bin/rm -rf grading_$$
        mkdir grading_$$
        cd grading_$$
        cp ../my402list.c .
        cp $srcdir/cs402.h .
        cp $srcdir/my402list.h .
        cp $srcdir/listtest.c .
        cp $srcdir/Makefile .
        make

        set seeds = ( 20 19 18 17 16 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 )

        #
        # for the following commands, each correct behavior gets 2 point
        # gets 2 points if "./listtest" command produces NOTHING
        # gets 0 point if "./listtest" command produces ANY output
        #
        foreach f (1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20)
            echo "===> test_$f"
            ./listtest -seed=$seeds[$f]
        end
        cd ..
#
# Clean up temporary directory
#
/bin/rm -rf grading_$$