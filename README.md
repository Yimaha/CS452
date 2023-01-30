# CS452, Group JCCH's Repo

this course is really hard

## Group Members

* Justin Cai (t28cai@uwaterloo.ca)
* Colin He (cqhe@uwaterloo.ca)

## K1 Build Instructions

To build the main executable:

* Use git to download the code from our repository:

    ```git
    git clone ist-git@git.uwaterloo.ca:t28cai/cs452-microkernel.git
    ```

* Checkout the `k1` branch:

    ```git
    git checkout k1
    ```

* `cd` into the k1 directory:

    ```sh
    cd cs452-microkernel/k1
    ```

* Run `make` to build the code into a loadable, executable image.

To run the executable on a Raspberry Pi:

* Run

    ```sh
    /u/cs452/public/tools/setupTFTP.sh <barcode> kernel8.img
    ```

    to load the code onto the Raspberry Pi with barcode \<barcode\>. The barcode will be one of (currently) four, of the form CS01754[0-3], although only machines CS01754[01] are connected to a train set.
* Wait for output to appear on the screen.

A report summarizing and outlining our work can be found in cs452-microkernel/k1/K1_report.pdf.
