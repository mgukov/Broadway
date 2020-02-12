
  // Module.onRuntimeInitialized = () => {
  //   if (onLoaded) {
  //     onLoaded(Module);
  //   }
  // };

addOnPostRun(() => {
	if (onLoaded) {
	  onLoaded(Module);
	}
});

}