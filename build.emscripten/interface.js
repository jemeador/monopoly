const boardWidth = 960;
const boardHeight = boardWidth;
const spaceToBoardRatio = 0.125;
const sideCount = 4;
const spacesPerSide = 10;
const spaceHeight = spaceToBoardRatio * boardHeight;
const spaceWidth = (boardWidth - (2 * spaceHeight)) / (spacesPerSide - 1);
const pxPosOffset = spaceWidth - spaceHeight;
const iconToSpaceRatio = 0.8;
const pieceToSpaceRatio = 0.125;
const priceToSpaceRatio = 0.2;
const colorBannerToSpaceRatio = 0.2;
const spaceIconSize = iconToSpaceRatio * spaceWidth;
const pieceIconSize = pieceToSpaceRatio * spaceHeight;
const priceFontSize = priceToSpaceRatio * spaceWidth;
const colorBannerHeight = colorBannerToSpaceRatio * spaceHeight;

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

        var boardColor = '#CDE6D0'; // Monopoly board teal
        var basicIconColor = '#726E6D'; // Smokey Gray
        var rollButton = document.getElementById("rollButton");
        var buyPropertyButton = document.getElementById("buyPropertyButton");
        var auctionPropertyButton = document.getElementById("auctionPropertyButton");
        var useGetOutOfJailFreeCardButton = document.getElementById("useGetOutOfJailFreeCardButton");
        var payBailButton = document.getElementById("payBailButton");
        var bidButton = document.getElementById("bidButton");
        var declineBidButton = document.getElementById("declineBidButton");
        var endTurnButton = document.getElementById("endTurnButton");
        var resignButton = document.getElementById("resignButton");
        var startManageButton = document.getElementById("startManageButton");
        var finishManageButton = document.getElementById("finishManageButton");
        var mortgageButton = document.getElementById("mortgageButton");
        var unmortgageButton = document.getElementById("unmortgageButton");
        var nextBid = function (state_) {
            return state_.get_current_auction ().highestBid + 10;
        }
        var getPlayerIndex = function (state_) {
            return state_.get_controlling_player_index();
        };
        var manageModeOn = false;
        var selectedProperty = -1;

        // Ensure font awesome is loaded before we draw icons
        setTimeout(start, 100) // Yucky, we should actually wait until font awesome is loaded
        function start() {
            var interface = new JavascriptInterface;
            var game = new Module.Game(interface);

            let dieIconName = (faceValue) => {
                switch (faceValue) {
                    case 1: return "one";
                    case 2: return "two";
                    case 3: return "three";
                    case 4: return "four";
                    case 5: return "five";
                    case 6: return "six";
                }
                return "d6";
            }
            let updateDiceDisplay = () => {
                var d1 = document.getElementById("die1");
                var d2 = document.getElementById("die2");
                var diceValues = gameState.get_last_dice_roll();

                let updateDieDisplay = (dieElement, value) => {
                    if (Module.can) {
                        dieElement.setAttribute("class", "dice fas fa-dice-d6");
                    }
                    else {
                        dieElement.setAttribute("class", "dice fas fa-dice-" + dieIconName (value));
                    }
                }
                updateDieDisplay(d1, diceValues.first);
                updateDieDisplay(d2, diceValues.second);
            }

            let getSpaceClicked = (canvas, event) => {
                const boundingRect = canvas.getBoundingClientRect();
                const x = event.clientX - boundingRect.left;
                const y = event.clientY - boundingRect.top;
                const top = y <= spaceHeight;
                const bot = y >= boardHeight - spaceHeight;
                const left = x <= spaceHeight;
                const right = x >= boardWidth - spaceHeight;

                let side = 0;
                let pxPos = 0;
                if (bot && !left) {
                    side = 0;
                    pxPos = (boardWidth - x) + pxPosOffset;
                }
                if (left && !top) {
                    side = 1;
                    pxPos = (boardHeight - y) + pxPosOffset
                }
                else if (top && !right) {
                    side = 2;
                    pxPos = x + pxPosOffset;
                }
                else if (right && !bot) {
                    side = 3;
                    pxPos = y + pxPosOffset;
                }

                if (pxPos < 0) {
                    pxPos = 0;
                }
                const indexAlongSide = Math.floor(pxPos / spaceWidth);
                const spaceIndex = side * 10 + indexAlongSide;
                const space = Module.index_to_space(spaceIndex);
                return space;
            }

            let selectProperty = (property) => {
                selectedProperty = property;
                updateInputButtons();
                paintBoard(gameState);
            }

            var canvas = document.getElementById("boardCanvas");

            canvas.addEventListener("click", function (e) {
                if (manageModeOn) {
                    const space = getSpaceClicked(canvas, e);
                    const property = Module.space_to_property(space);
                    if (property != -1) {
                        const ownerIndex = gameState.get_property_owner_index(property);
                        if (ownerIndex == getPlayerIndex(gameState)) {
                            if (selectedProperty != property) {
                                selectProperty(property)
                                return;
                            }
                        }
                    }
                }
                selectProperty(-1);
            }, false);

            rollButton.onclick = function () {
                interface.roll_dice(getPlayerIndex (gameState));
                game.process();
                updateDiceDisplay();
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
            resignButton.onclick = function () {
                interface.resign(getPlayerIndex (gameState));
                game.process();
            };
            startManageButton.onclick = function () {
                manageModeOn = true;
                updateInputButtons(gameState);
                paintBoard(gameState);
            };
            finishManageButton.onclick = function () {
                manageModeOn = false;
                selectedProperty = -1;
                updateInputButtons(gameState);
                paintBoard(gameState);
            };
            mortgageButton.onclick = function () {
                interface.mortgage_property(getPlayerIndex (gameState), selectedProperty);
                game.process();
            };
            unmortgageButton.onclick = function () {
                interface.unmortgage_property(getPlayerIndex (gameState), selectedProperty);
                game.process();
            };
        }
        function updateInputButtons(interface) {

            var setButtonVisibility = (button, visible) => {
                if (visible) {
                    button.style.display = 'block';
                }
                else {
                    button.style.display = 'none';
                }
            };
            const thisPlayerIndex = getPlayerIndex(gameState);
            setButtonVisibility(rollButton, ! manageModeOn && gameState.check_if_player_is_allowed_to_roll(thisPlayerIndex));
            setButtonVisibility(buyPropertyButton, ! manageModeOn && gameState.check_if_player_is_allowed_to_buy_property(thisPlayerIndex));
            setButtonVisibility(auctionPropertyButton, ! manageModeOn && gameState.check_if_player_is_allowed_to_auction_property(thisPlayerIndex));
            setButtonVisibility(useGetOutOfJailFreeCardButton, ! manageModeOn && gameState.check_if_player_is_allowed_to_use_get_out_jail_free_card(thisPlayerIndex));
            setButtonVisibility(payBailButton, ! manageModeOn && gameState.check_if_player_is_allowed_to_pay_bail(thisPlayerIndex));
            setButtonVisibility(bidButton, ! manageModeOn && gameState.check_if_player_is_allowed_to_bid(thisPlayerIndex, nextBid(gameState)));
            setButtonVisibility(declineBidButton, ! manageModeOn && gameState.check_if_player_is_allowed_to_decline_bid(thisPlayerIndex));
            setButtonVisibility(endTurnButton, ! manageModeOn && gameState.check_if_player_is_allowed_to_end_turn(thisPlayerIndex));
            setButtonVisibility(resignButton, gameState.check_if_player_is_allowed_to_resign(thisPlayerIndex));
            setButtonVisibility(startManageButton, ! manageModeOn && gameState.get_controlling_player_index() == thisPlayerIndex);
            setButtonVisibility(finishManageButton, gameState.get_controlling_player_index() == thisPlayerIndex && manageModeOn);
            setButtonVisibility(mortgageButton, gameState.check_if_player_is_allowed_to_mortgage(thisPlayerIndex, selectedProperty));
            setButtonVisibility(unmortgageButton, gameState.check_if_player_is_allowed_to_unmortgage(thisPlayerIndex, selectedProperty));
        }
        function paintBoard(gameState) {

            const playerIcons = [
                "\uf21a", // fa-ship
                "\uf544", // fa-robot
                "\uf0f4", // fa-coffee
                "\uf5fc", // fa-laptop-code
            ];

            const playerColors = [
                '#4863A0', // Steel Blue
                '#4E8975', // Sea Green
                '#6F4E37', // Coffee
                '#E18B6B', // Caramel
            ];

            let translateToSpace = (ctx, space) => {
                const index = Module.space_to_index(space);
                const side = Math.floor (index / 10);
                const pos = index % 10;

                // Rotate about center
                ctx.translate(boardWidth / 2, boardHeight / 2);
                ctx.rotate(side * Math.PI / 2);
                ctx.translate(-boardWidth / 2, -boardHeight / 2);

                // Go to top left corner of space on this side (should be facing the up at this rotation)
                ctx.translate(boardWidth - (spaceHeight + spaceWidth * pos), boardHeight - spaceHeight);
            }

            let drawPlayerPiece = (ctx, playerIndex, space) => {
                const icon = playerIcons[playerIndex];
                const color = playerColors[playerIndex];

                ctx.save();
                translateToSpace(ctx, space);
                ctx.translate(spaceWidth / 2, spaceHeight / 2);
                const x = (playerIndex % 2 == 0 ? -1 : 1) * spaceWidth / 4;
                const y = (playerIndex < 2 == 0 ? -1 : 1) * spaceHeight / 6;
                ctx.translate(x, y);
                ctx.beginPath();
                ctx.arc(0, 0, pieceIconSize / 2 + spaceWidth/10, 0, 2 * Math.PI, false);
                ctx.fillStyle = color;
                ctx.strokeStyle = 'Black';
                ctx.fill();
                ctx.stroke();
                drawIcon(ctx, 'White', pieceIconSize,  icon);
                ctx.restore();
            }

            let drawPlayerPieces = (ctx) => {
                for (let p = 0; p < gameState.get_player_count(); ++p) {
                    const space = gameState.get_player_position(p);
                    drawPlayerPiece(ctx, p, space);
                }
            };

            let drawPlayerFunds = (ctx, space) => {
                let table = document.getElementById("playerTable");
                table.innerHTML = "";
                let playerTableData = [];
                const controllingPlayerIndex = gameState.get_controlling_player_index();
                
                for (let p = 0; p < gameState.get_player_count(); ++p) {
                    if (gameState.get_player_eliminated(p))
                        continue;
                    let row = table.insertRow();
                    let playerData = [Module.player_name(p), "$" + gameState.get_player_funds(p)];
                    for (key in playerData) {
                        let cell = row.insertCell();
                        let text = document.createTextNode (playerData[key]);
                        cell.appendChild(text);
                        if (controllingPlayerIndex == p) {
                            cell.style.backgroundColor = playerColors[p];
                            cell.style.color = 'white';
                        }
                        else {
                            cell.style.color = playerColors[p];
                        }
                    }
                }
            }

            let groupToColor = (group) => {
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

            let isCornerSpace = (space) => {
                return space == Module.Space.Go
                    || space == Module.Space.Jail
                    || space == Module.Space.FreeParking
                    || space == Module.Space.GoToJail;
            }

            let drawPropertySpace = (ctx, space) => {
                const property = Module.space_to_property(space);
                const group = Module.property_group(property);
                const color = groupToColor(group);
                if (color) {
                    drawColorBar(ctx, color);
                }
                else {
                    drawNormalSpace(ctx, space);
                }
                drawPropertyPrice(ctx, property);
                const playerIndex = gameState.get_property_owner_index(property);
                if (playerIndex != -1) {
                    if (!manageModeOn) {
                        drawPropertyOwnershipIndicator(ctx, playerIndex);
                    }
                    else if (property == selectedProperty){
                        drawSelectionIndicator(ctx);
                    }
                }
                if (gameState.get_property_is_mortgaged(property)) {
                    drawMortgagedIndicator(ctx);
                }
            }

            let drawColorBar = (ctx, color) => {
                ctx.save();
                ctx.fillStyle = color;
                ctx.strokeStyle = 'Black';
                ctx.beginPath();
                ctx.rect(0, 0, spaceWidth, colorBannerHeight);
                ctx.closePath();
                ctx.fill();
                ctx.stroke();
                ctx.restore();
            }

            let drawPropertyPrice = (ctx, property) => {
                ctx.fillStyle = "Black";
                ctx.textAlign = 'center';
                ctx.textBaseline = 'bottom';
                ctx.font = priceFontSize + "px Righteous";
                ctx.fillText("$" + Module.price_of_property(property), spaceWidth / 2, spaceHeight - priceFontSize / 2);
            }

            let spaceToIcon = (space) => {
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

            let drawIcon = (ctx, color, pixelSize, icon) => {
                ctx.fillStyle = color;
                ctx.textAlign = 'center';
                ctx.textBaseline = 'middle';
                ctx.font = pixelSize + "px FontAwesome";
                ctx.fillText(icon, 0, 0);
            }

            let operateOnSpaces = (ctx, drawFunction) => {
                for (let s = 0; s < sideCount; ++s) {
                    ctx.translate(0, boardHeight - spaceHeight);
                    for (var i = 0; i < spacesPerSide; ++i) {
                        const spaceIndex = s * 10 + i;
                        const isCornerSpace = spaceIndex % 10 == 0;
                        const space = Module.index_to_space(spaceIndex);
                        ctx.save();
                        ctx.translate(boardWidth - (spaceHeight + spaceWidth * i), 0);
                        drawFunction(ctx, space);
                        ctx.restore();
                    }
                    ctx.translate(0, spaceHeight - boardHeight);
                    ctx.translate(boardWidth / 2, boardHeight / 2);
                    ctx.rotate(Math.PI / 2);
                    ctx.translate(-boardWidth / 2, -boardHeight / 2);
                }
            }

            let drawNormalSpace = (ctx, space) => {
                ctx.save();
                ctx.translate(spaceWidth / 2, spaceHeight / 2);
                drawIcon(ctx, basicIconColor, spaceIconSize, spaceToIcon(space));
                ctx.restore();
            }

            let drawCornerSpace = (ctx, space) => {
                ctx.save();
                ctx.translate(spaceHeight / 2, spaceHeight / 2);
                ctx.rotate(-Math.PI / 4);
                drawIcon(ctx, basicIconColor, spaceIconSize, spaceToIcon(space));
                ctx.restore();
            }

            let drawSpace = (ctx, space) => {
                ctx.save();
                ctx.beginPath();
                ctx.fillStyle = boardColor;
                if (isCornerSpace(space)) {
                    ctx.rect(0, 0, spaceHeight, spaceHeight);
                }
                else {
                    ctx.rect(0, 0, spaceWidth, spaceHeight);
                }

                ctx.closePath();
                ctx.fill();
                ctx.stroke();
                ctx.restore();
                if (isCornerSpace(space)) {
                    drawCornerSpace(ctx, space);
                }
                else {
                    if (Module.space_is_property(space)) {
                        drawPropertySpace(ctx, space);
                    }
                    else {
                        drawNormalSpace(ctx, space);
                    }
                }
            }

            let buildPathWithOwnedSpaces = (ctx, space) => {
                if (! Module.space_is_property(space)) {
                    return;
                }
                const property = Module.space_to_property(space);
                const owner = gameState.get_property_owner_index(property);
                if (owner == getPlayerIndex(gameState)) {
                    ctx.rect(0,0, spaceWidth, spaceHeight);
                }
            }

            let drawPropertyOwnershipIndicator = (ctx, player) => {
                ctx.save();
                ctx.beginPath();
                ctx.fillStyle = playerColors[player];
                ctx.strokeStyle = 'Black';
                ctx.rect(0, spaceHeight - colorBannerHeight, spaceWidth, spaceHeight);
                ctx.closePath();
                ctx.fill();
                ctx.stroke();
                ctx.restore();
            }

            let drawSelectionIndicator = (ctx) => {
                ctx.save();
                ctx.globalAlpha = 0.5;
                ctx.fillStyle = '#4891ff';
                ctx.fillRect(0, 0, spaceWidth, spaceHeight);
                ctx.restore();
            }

            let drawMortgagedIndicator = (ctx) => {
                const x = spaceWidth * 0.125;
                const y = spaceHeight / 2;
                const signWidth = spaceWidth * 0.75;
                const signHeight = spaceWidth * 0.25;
                ctx.save();
                ctx.fillStyle = 'Red';
                ctx.strokeStyle = 'Black';
                ctx.beginPath();
                ctx.rect(x, y, signWidth, signHeight);
                ctx.closePath();
                ctx.fill();
                ctx.stroke();
                ctx.textAlign = 'center';
                ctx.textBaseline = 'middle';
                ctx.fillStyle = 'White';
                ctx.translate(x - signWidth / 2, y - signHeight / 2);
                ctx.font = signHeight * 0.6 + "px Righteous";
                ctx.fillText("CLOSED", signWidth, signHeight);
                ctx.restore();
            }

            var canvas = document.getElementById("boardCanvas");

            var ctx = canvas.getContext("2d");
            ctx.globalCompositeOperation = "source-over";

            ctx.save();
            ctx.clearRect(0, 0, boardWidth, boardHeight);
            ctx.strokeRect(0, 0, boardWidth, boardHeight);
            operateOnSpaces(ctx, drawSpace)
            drawPlayerPieces(ctx);
            drawPlayerFunds(ctx);
            ctx.restore();

            if (manageModeOn) {
                ctx.save();
                ctx.globalCompositeOperation = "saturation";
                ctx.beginPath();
                ctx.rect(0, 0, boardWidth, boardHeight);
                operateOnSpaces(ctx, buildPathWithOwnedSpaces)
                ctx.closePath();
                ctx.clip("evenodd");
                ctx.fillStyle = 'White';
                ctx.fillRect(0, 0, boardWidth, boardHeight);
                ctx.restore();
            }
        }
    }
}
