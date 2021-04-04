import random
import psutil
import os
from player import BlackJackPlayer


class BlackJack:

    def __init__(self):
        player = BlackJackPlayer('player')
        ai = BlackJackPlayer('ai')
        # self.aiObservedCard = []
        # self.playerHand = []
        # self.scorePlayer = 0
        # self.scoreAI = 0
        # self.cnt = 0
        # self.lstAIHand = []
        # self.type = None
        # self.card = None

    def createMemoizationTable(self):

        self.tblBestSelect = []
        self.tblSelect = [[] for i in range(65)]
        self.tblRemaining = [[] for i in range(65)]
        if self.scoreAI == 0:
            self.tblSelect[self.scoreAI] = []
            self.tblRemaining[self.scoreAI] = self.aiObservedCard
        else:
            self.tblSelect[self.scoreAI] = self.lstAIHand
            self.tblRemaining[self.scoreAI] = self.aiObservedCard
        for i in range(self.scoreAI, len(self.tblSelect)):
            for card in self.tblRemaining[i]:
                value = self.dic[card.split('-')[0]]
                if self.tblSelect[i + value] == []:
                    self.tblSelect[i + value] = self.tblSelect[i] + [card]
                    x = self.tblRemaining[i][:]
                    x.remove(card)
                    self.tblRemaining[i + value] = x
        for i in range(21, 0, -1):
            if self.tblSelect[i] != []:
                print("INSIDE AI : AI Expected Total Score : " + str(i))
                print("INSIDE AI : AI Selected Cards : " + str(self.tblSelect[i][:]))
                self.tblBestSelect = self.tblSelect[i]
                break

    def shuffleCard(self):
        cardValue = ['A', '2', '3', '4', '5', '6', '7', '8', '9', '10', 'J', 'Q', 'K']
        shape = ['Spade', 'Diamond', 'Clover', 'Heart']
        card = []
        self.dic = {'A': 1, 'J': 11, 'Q': 12, 'K': 13}
        for n in range(2, 11):
            self.dic[str(n)] = n
        for i in list(cardValue):
            for j in list(shape):
                card.append(str(i) + '-' + str(j))
        random.shuffle(card)

        self.aiObservedCard = card[:5]
        self.card = card

    def playerHitOrPass(self):
        self.type = input()
        if self.type == 'H':
            self.playerHand.append(self.aiObservedCard[0])
            self.scorePlayer = self.scorePlayer + self.dic[self.aiObservedCard[0].split('-')[0]]
            del self.aiObservedCard[0]
            del self.card[0]
        elif self.type == 'P':
            pass
        else:
            print("(H)it or (P)ass ??? Type!")
            self.playerHitOrPass()

    def aiHitOrPass(self):
        print("AI Observed Cards : " + str(self.aiObservedCard))
        self.createMemoizationTable()
        print("AI Cards to Play : " + self.aiObservedCard[0])

        if self.type == 'P':
            if self.scorePlayer > self.scoreAI:
                print("INSIDE AI : AI cannot pass because Player will win if AI pass")
                self.hit()
            else:
                self.skip()
        elif self.aiObservedCard[0] in self.tblBestSelect:
            self.hit()
        # both pass
        else:
            self.skip()

    def hit(self):
        print("AI Hit")
        self.lstAIHand.append(self.aiObservedCard[0])
        self.scoreAI = self.scoreAI + self.dic[self.aiObservedCard[0].split('-')[0]]
        del self.card[0]
        self.aiType = "H"

    def skip(self):
        print("AI Pass")
        self.aiType = "P"

    def winOrLose(self):
        if self.scoreAI > 21:
            print("AI Lose!")
        elif self.scorePlayer > 21:
            print("Player Lose!")
        else:
            if self.scoreAI > self.scorePlayer:
                print("Player Lose!")
            elif self.scoreAI == self.scorePlayer:
                print("Player and AI Tie!")
            else:
                print("AI Lose!")

    def run(self):
        self.shuffleCard()
        while (self.scorePlayer < 21 and self.scoreAI < 21) and not (self.type == 'P' and self.aiType == 'P'):
            print("--------------------------------------------------")
            print("Round : " + str(self.cnt + 1))
            print(self.card[:5])
            print("AI Current Score : " + str(self.scoreAI))
            print("AI's Hand : " + str(self.lstAIHand))
            print("Player Current Score : " + str(self.scorePlayer))
            print("Player's Hand : " + str(self.playerHand))
            print("Player Cards to Play : " + str(self.aiObservedCard[0]))
            print("(H)it or (P)ass ??? Type!")
            self.playerHitOrPass()
            self.aiHitOrPass()
            self.cnt = self.cnt + 1
            self.aiObservedCard = self.card[:5]
            print("AI Current Score : " + str(self.scoreAI))
            print("Player Current Score : " + str(self.scorePlayer))
        self.winOrLose()


if __name__ == '__main__':
    w = BlackJack()
    w.run()
