import random


class BlackJackPlayer:
    def __init__(self, player_type: str):
        self.player_type: str = self.player_type
        self.score: int = 0
        self.card_list: list = 0
        self.cnt: int = 0

    def HitOrPass(self):
        hit_or_pass = input()
        if hit_or_pass == 'H':
            pass
        elif hit_or_pass == 'P':
            pass
        else:
            print('(H)it or (P)ass ??? Type!')
            self.HitOrPass()
