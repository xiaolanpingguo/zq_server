1.如果重新进行了clone了工程后，要在linux编译proto的话，需要重新删除现在已有的解压出来的proto文件夹，在来重新解压protobuf-all-3.6.1.tar.gz，然后再进行编译
	原因在于为了在git省空间，我在编译完成proto后，进行了make clean，然后不知道为什么在./configurer就不成功了，所以只能重新解压
	这样可能有点麻烦，因为重新解压后，win上面的工程又要重新cmake一下再加入解决方案，有点操蛋