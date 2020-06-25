### Run example of simple one-sided RDMA

`cd /path/to/r2/; cmake .; make oserver;make oclient;`

Suppose the server is at 1.1.1.1, so first at server, run: `./oserver`;

Then at client run `./oclient --server_host=1.1.1.1`; Then all is done. 

The codes are in `./examples/basic_onesided_rdma`. 


## License

R2 is released under the [Apache 2.0 license](http://www.apache.org/licenses/LICENSE-2.0.html).

As R2 is the refined execution framework of DrTM+H, 
if you use R2 in your research, please cite our paper:

    @inproceedings {drtmhosdi18,
        author = {Xingda Wei and Zhiyuan Dong and Rong Chen and Haibo Chen},
        title = {Deconstructing RDMA-enabled Distributed Transactions: Hybrid is Better!},
        booktitle = {13th {USENIX} Symposium on Operating Systems Design and Implementation ({OSDI} 18)},
        year = {2018},
        isbn = {978-1-939133-08-3},
        address = {Carlsbad, CA},
        pages = {233--251},
        url = {https://www.usenix.org/conference/osdi18/presentation/wei},
        publisher = {{USENIX} Association},
        month = oct,
    }
