var Module = {
    onRuntimeInitialized: function () {
        var c = document.getElementById("boardCanvas");
        var ctx = c.getContext("2d");
        ctx.font = "30px Arial";
        for (var i = 0; i < Module.NumberOfSpaces; i++) {
            var space = Module.index_to_space(i);
            if (Module.space_is_property(space)) {
                console.log(Module.price_of_property(Module.space_to_property(space)));
            }
        }
        var price = Module.price_of_property(Module.Property.Brown_1);
        ctx.fillText(price, 10, 50);
    }
}
