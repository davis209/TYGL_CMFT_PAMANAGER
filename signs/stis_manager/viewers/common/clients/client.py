from abc import ABC, abstractmethod
from common.transactive.message import Message
from common.transactive.template import Template
from common.transactive.location import Location
from common.transactive.pid import Pid


class Client(ABC):

    @abstractmethod
    def get_messages(self, location: str) -> (list[Message], str):
        pass

    @abstractmethod
    def clear_messages(self, destinations) -> str:
        pass

    @abstractmethod
    def get_templates(self, location: str) -> (list[Template], str):
        pass

    @abstractmethod
    def remove_templates(self, templates) -> str:
        pass

    @abstractmethod
    def get_locations(self) -> (list[Location], str):
        pass

    @abstractmethod
    def get_pids(self) -> (list[Pid], str):
        pass
