Visual Studio Locator
=====================

Over the years Visual Studio could be discovered using registry keys, but with recent changes to the deployment and extensibility models a new method is needed to discover possibly more than once installed instance. These changes facilitate a smaller, faster default install complimented by on-demand install of other workloads and components.

_vswhere_ is designed to be a redistributable, single-file executable that can be used in build or deployment scripts to find where Visual Studio - or other products in the Visual Studio family - is located. For example, if you know the relative path to MSBuild, you can find the root of the Visual Studio install and combine the paths to find what you need.

You can emit different formats for information based on what your scripts can consume, including plain text, JSON, and XML. Pull requests may be accepted for other common formats as well.

## Feedback

To file issues or suggestions, please use the [Issues](https://github.com/Microsoft/vswhere/issues) page for this project on GitHub.

## License

This project is licensed under the [MIT license](LICENSE.txt).
