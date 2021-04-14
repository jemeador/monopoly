var Module = {
    onRuntimeInitialized: function () {
        var gameState = new Module.GameState;
        var JavascriptInterface = Module.SimpleInterface.extend("SimpleInterface", {
            waitForInput: function () {
                updateInputButtons(gameState);
                paintBoard(gameState);
            },
            // C++ override
            get_setup: function () {
                console.log(Module.GameSetup);
                return {
                    seed: "monopolyRandomSeed!",
                    playerCount: 4,
                    startingFunds: 1500,
                };
            },
            // C++ override
            update: function (state_) {
                gameState.assign (state_);
                this.waitForInput();
            },
        });

        var rollButton = document.getElementById("rollButton");
        var buyPropertyButton = document.getElementById("buyPropertyButton");
        var auctionPropertyButton = document.getElementById("auctionPropertyButton");
        var useGetOutOfJailFreeCardButton = document.getElementById("useGetOutOfJailFreeCardButton");
        var payBailButton = document.getElementById("payBailButton");
        var bidButton = document.getElementById("bidButton");
        var declineBidButton = document.getElementById("declineBidButton");
        var endTurnButton = document.getElementById("endTurnButton");
        var nextBid = function (state_) {
            return state_.get_current_auction ().highestBid + 10;
        }

        // Ensure font awesome is loaded before we draw icons
        setTimeout(start, 100) // Yucky, we should actually wait until font awesome is loaded
        function start() {
            var interface = new JavascriptInterface;
            var game = new Module.Game(interface);

            var getPlayerIndex = function (state_) {
                return state_.get_controlling_player_index();
            };

            rollButton.onclick = function () {
                interface.roll_dice(getPlayerIndex (gameState));
                game.process();
            };
            buyPropertyButton.onclick = function () {
                interface.buy_property(getPlayerIndex (gameState));
                game.process();
            };
            auctionPropertyButton.onclick = function () {
                interface.auction_property(getPlayerIndex (gameState));
                game.process();
            };
            useGetOutOfJailFreeCardButton.onclick = function () {
                interface.use_get_out_of_jail_free_card(getPlayerIndex (gameState));
                game.process();
            };
            payBailButton.onclick = function () {
                interface.pay_bail(getPlayerIndex (gameState));
                game.process();
            };
            bidButton.onclick = function () {
                interface.bid(getPlayerIndex (gameState), nextBid(gameState));
                game.process();
            };
            declineBidButton.onclick = function () {
                interface.decline_bid(getPlayerIndex (gameState));
                game.process();
            };
            endTurnButton.onclick = function () {
                interface.end_turn(getPlayerIndex (gameState));
                game.process();
            };
        }
        function updateInputButtons(interface) {
            let controllingPlayerIndex = gameState.get_controlling_player_index();

            var setButtonVisibility = (button, visible) => {
                if (visible) {
                    button.style.display = 'block';
                }
                else {
                    button.style.display = 'none';
                }
            };
            setButtonVisibility(rollButton, gameState.check_if_player_is_allowed_to_roll(controllingPlayerIndex));
            setButtonVisibility(buyPropertyButton, gameState.check_if_player_is_allowed_to_buy_property(controllingPlayerIndex));
            setButtonVisibility(auctionPropertyButton, gameState.check_if_player_is_allowed_to_auction_property(controllingPlayerIndex));
            setButtonVisibility(useGetOutOfJailFreeCardButton, gameState.check_if_player_is_allowed_to_use_get_out_jail_free_card(controllingPlayerIndex));
            setButtonVisibility(payBailButton, gameState.check_if_player_is_allowed_to_pay_bail(controllingPlayerIndex));
            setButtonVisibility(bidButton, gameState.check_if_player_is_allowed_to_bid(controllingPlayerIndex, nextBid(gameState)));
            setButtonVisibility(declineBidButton, gameState.check_if_player_is_allowed_to_decline_bid(controllingPlayerIndex));
            setButtonVisibility(endTurnButton, gameState.check_if_player_is_allowed_to_end_turn(controllingPlayerIndex));
        }
        function paintBoard(gameState) {
            var group_to_color = (group) => {
                switch (group) {
                    case Module.PropertyGroup.Brown:
                        return "Brown";
                    case Module.PropertyGroup.LightBlue:
                        return "LightBlue";
                    case Module.PropertyGroup.Magenta:
                        return "Magenta";
                    case Module.PropertyGroup.Orange:
                        return "Orange";
                    case Module.PropertyGroup.Red:
                        return "Red";
                    case Module.PropertyGroup.Yellow:
                        return "Yellow";
                    case Module.PropertyGroup.Green:
                        return "Green";
                    case Module.PropertyGroup.Blue:
                        return "Blue";
                    default:
                        return "";
                }
            }
            var space_to_icon = (space) => {
                switch (space) {
                    case Module.Space.Go:
                        return "\uf30a";
                    case Module.Space.Jail:
                        return "\uf6d9";
                    case Module.Space.FreeParking:
                        return "\uf540";
                    case Module.Space.GoToJail:
                        return "\uf0a6";
                    case Module.Space.CommunityChest_1:
                    case Module.Space.CommunityChest_2:
                    case Module.Space.CommunityChest_3:
                        return "\uf51e";
                    case Module.Space.Chance_1:
                    case Module.Space.Chance_2:
                    case Module.Space.Chance_3:
                        return "\uf128";
                    case Module.Space.IncomeTax:
                        return "\uf4c0";
                    case Module.Space.LuxuryTax:
                        return "\uf3a5";
                    case Module.Space.Railroad_1:
                    case Module.Space.Railroad_2:
                    case Module.Space.Railroad_3:
                    case Module.Space.Railroad_4:
                        return "\uf238";
                    case Module.Space.Utility_1:
                        return "\uf0eb";
                    case Module.Space.Utility_2:
                        return "\ue005";
                    default:
                        return "";
                }
            }

            var c = document.getElementById("boardCanvas");
            var ctx = c.getContext("2d");
            const boardWidth = c.width - 2;
            const boardHeight = boardWidth;
            ctx.save();
            ctx.clearRect(0, 0, boardWidth, boardHeight);
            const sideCount = 4;
            const spacesPerSide = 10;
            const spaceToBoardRatio = 0.1365;
            const iconToSpaceRatio = 0.8;
            const priceToSpaceRatio = 0.2;
            const colorBannerToSpaceRatio = 0.2;
            const spaceHeight = spaceToBoardRatio * boardHeight;
            const spaceWidth = (boardWidth - (2 * spaceHeight)) / (spacesPerSide - 1);
            const spaceIconSize = iconToSpaceRatio * spaceWidth;
            const priceFontSize = priceToSpaceRatio * spaceWidth;
            const colorBannerHeight = colorBannerToSpaceRatio * spaceHeight;

            ctx.textAlign = 'center';
            ctx.strokeRect(0, 0, boardWidth, boardHeight);

            for (var s = 0; s < sideCount; ++s) {
                ctx.translate(0, boardHeight - spaceHeight);
                ctx.strokeRect(0, 0, boardWidth, spaceHeight);
                for (var i = 0; i < spacesPerSide; ++i) {
                    const spaceIndex = s * 10 + i;
                    const space = Module.index_to_space(spaceIndex);
                    const isCornerSpace = i == 0;
                    ctx.save();
                    ctx.translate(boardWidth - (spaceHeight + spaceWidth * i), 0);
                    if (!isCornerSpace) {
                        ctx.strokeRect(0, 0, spaceWidth, spaceHeight);
                    }
                    if (Module.space_is_property(space)) {
                        const property = Module.space_to_property(space);
                        const group = Module.property_group(property);
                        const color = group_to_color(group);
                        if (color) {
                            ctx.fillStyle = color;
                            ctx.beginPath();
                            ctx.rect(0, 0, spaceWidth, colorBannerHeight);
                            ctx.closePath();
                            ctx.fill();
                            ctx.stroke();
                        }
                        ctx.fillStyle = "Black";
                        ctx.textBaseline = 'bottom';
                        ctx.font = priceFontSize + "px Arial";
                        ctx.fillText("$" + Module.price_of_property(property), spaceWidth / 2, spaceHeight);
                    }
                    if (iconCode = space_to_icon(space)) {
                        ctx.save();
                        ctx.fillStyle = "Black";
                        ctx.textBaseline = 'middle';
                        ctx.font = spaceIconSize + "px FontAwesome";
                        if (isCornerSpace) {
                            ctx.translate(spaceHeight / 2, spaceHeight / 2);
                            ctx.rotate(-Math.PI / 4);
                        }
                        else {
                            ctx.translate(spaceWidth / 2, spaceHeight / 2);
                        }
                        ctx.fillText(iconCode, 0, 0);
                        ctx.restore();
                    }
                    ctx.restore();
                }
                ctx.translate(0, spaceHeight - boardHeight);
                ctx.translate(boardWidth / 2, boardHeight / 2);
                ctx.rotate(Math.PI / 2);
                ctx.translate(-boardWidth / 2, -boardHeight / 2);
            }

            ctx.font = 20 + "px Arial";
            ctx.translate(boardWidth / 2, boardHeight / 2);
            ctx.fillStyle = "Black";
            ctx.textBaseline = 'middle';
            ctx.fillText(Module.player_name(gameState.get_controlling_player_index ()), 0, 0);
            ctx.restore();
        }
    }
}
